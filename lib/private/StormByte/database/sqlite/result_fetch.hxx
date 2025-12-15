#pragma once

#include <StormByte/database/rows.hxx>

#include <sqlite3.h>

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @brief Steps through the results of a SQLite statement and fills the provided Row and Rows objects.
	 * @param stmt The SQLite statement to step through.
	 * @return ExpectedRows containing the result rows or an error.
	 */
	// Windows needs this exported public even if it's private, this is why it is inline
	inline ExpectedRows StepResults(sqlite3_stmt* stmt) noexcept {
		if (!stmt) {
			return Unexpected<QueryException>(ExecuteError("Invalid SQLite statement provided."));
		}

		Rows rows;
		int rc;
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			Row row;
			int colCount = sqlite3_column_count(stmt);
			for (int i = 0; i < colCount; i++) {
				const char* colName = sqlite3_column_name(stmt, i);
				switch (sqlite3_column_type(stmt, i)) {
					case SQLITE_INTEGER: {
						sqlite3_int64 v = sqlite3_column_int64(stmt, i);
						if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
							row.Add(std::string(colName ? colName : ""), static_cast<long int>(v));
						else
							row.Add(std::string(colName ? colName : ""), static_cast<int>(v));
						break;
					}
					case SQLITE_FLOAT:
						row.Add(std::string(colName ? colName : ""), sqlite3_column_double(stmt, i));
						break;
					case SQLITE_TEXT: {
						const unsigned char* text = sqlite3_column_text(stmt, i);
						row.Add(std::string(colName ? colName : ""), std::string(reinterpret_cast<const char*>(text ? text : (const unsigned char*)"")));
						break;
					}
					case SQLITE_BLOB: {
						const std::byte* blobData = reinterpret_cast<const std::byte*>(sqlite3_column_blob(stmt, i));
						int blobSize = sqlite3_column_bytes(stmt, i);
						std::vector<std::byte> blobVec;
						if (blobData && blobSize > 0)
							blobVec.assign(blobData, blobData + blobSize);
						row.Add(std::string(colName ? colName : ""), std::move(blobVec));
						break;
					}
					case SQLITE_NULL:
					default:
						row.Add(std::string(colName ? colName : ""), nullptr);
						break;
				}
			}
			rows.Add(std::move(row));
		}

		if (rc == SQLITE_DONE) {
			return rows;
		}

		const char* errMsg = "Unknown SQLite error";
		if (sqlite3_db_handle(stmt))
			errMsg = sqlite3_errmsg(sqlite3_db_handle(stmt));
		return Unexpected<QueryException>(ExecuteError(errMsg ? errMsg : "Unknown SQLite error"));
	}
}