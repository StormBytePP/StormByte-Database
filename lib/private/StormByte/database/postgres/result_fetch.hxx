#pragma once

#include <StormByte/database/rows.hxx>
#include <libpq-fe.h>

namespace StormByte::Database::Postgres {
	inline ExpectedRows StepResults(PGresult* res) noexcept {
		if (!res)
			return Unexpected<QueryException>(ExecuteError("Invalid PGresult provided."));

		ExecStatusType st = PQresultStatus(res);
		if (st != PGRES_TUPLES_OK && st != PGRES_COMMAND_OK) {
			return Unexpected<QueryException>(ExecuteError(PQresultErrorMessage(res) ? PQresultErrorMessage(res) : "Unknown PG error"));
		}

		Rows rows;
		int nrows = PQntuples(res);
		int nfields = PQnfields(res);
		for (int r = 0; r < nrows; ++r) {
			Row row;
			for (int c = 0; c < nfields; ++c) {
				const char* colName = PQfname(res, c);
				if (PQgetisnull(res, r, c)) {
					row.Add(std::string(colName ? colName : ""), Value());
					continue;
				}
				Oid ftype = PQftype(res, c);
				const char* val = PQgetvalue(res, r, c);
				int vall = PQgetlength(res, r, c);
				switch (ftype) {
					case 16: { // bool
						bool b = false;
						if (val) {
							if (val[0] == 't' || val[0] == '1') b = true;
							else {
								std::string s(val);
								for (auto &ch : s) ch = static_cast<char>(std::tolower(ch));
								if (s == "true") b = true;
							}
						}
						row.Add(std::string(colName ? colName : ""), b);
						break;
					}

					case 20: // int8
					case 21: // int2
					case 23: { // int4
						long long v = 0;
						try { v = std::stoll(std::string(val)); } catch(...) { v = 0; }
						if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
							row.Add(std::string(colName ? colName : ""), static_cast<long int>(v));
						else
							row.Add(std::string(colName ? colName : ""), static_cast<int>(v));
						break;
					}
					case 700: // float4
					case 701: { // float8
						double d = 0.0;
						try { d = std::stod(std::string(val)); } catch(...) { d = 0.0; }
						row.Add(std::string(colName ? colName : ""), d);
						break;
					}
					case 17: { // bytea
						unsigned char* out = nullptr;
						size_t outlen = 0;
						out = PQunescapeBytea(reinterpret_cast<const unsigned char*>(val), &outlen);
						std::vector<std::byte> blob;
						if (out && outlen > 0) {
							blob.assign(reinterpret_cast<std::byte*>(out), reinterpret_cast<std::byte*>(out) + outlen);
						}
						if (out) PQfreemem(out);
						row.Add(std::string(colName ? colName : ""), std::move(blob));
						break;
					}
					default: {
						// default to text
						row.Add(std::string(colName ? colName : ""), std::string(val ? val : ""));
						break;
					}
				}
			}
			rows.Add(std::move(row));
		}

		return rows;
	}
}
