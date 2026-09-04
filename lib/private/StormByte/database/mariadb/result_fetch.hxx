/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte-Database.
*
* StormByte-Database is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* or later, as published by the Free Software Foundation.
*
* StormByte-Database is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte-Database. If not, see
* <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#pragma once

#include <StormByte/database/rows.hxx>

#include <limits>
#include <mysql.h>
#include <string>
#include <vector>

/**
 * @brief MariaDB backend of the Database module.
 */
namespace StormByte::Database::MariaDB {
	/**
	 * @brief Convert a MYSQL_RES into Rows (all rows stored client-side).
	 * @param res Result set (must not be null).
	 * @return Result rows or a QueryException.
	 */
	inline ExpectedRows StepResults(MYSQL_RES* res) noexcept {
		if (!res)
			return Unexpected<QueryException>(ExecuteError("Invalid MYSQL_RES provided."));

		Rows rows;
		const int nrows = static_cast<int>(mysql_num_rows(res));
		const int nfields = mysql_num_fields(res);

		for (int r = 0; r < nrows; ++r) {
			MYSQL_ROW row = mysql_fetch_row(res);
			unsigned long* lengths = mysql_fetch_lengths(res);
			Row prow;

			for (int c = 0; c < nfields; ++c) {
				MYSQL_FIELD* field = mysql_fetch_field_direct(res, c);
				const char* colName = field ? field->name : nullptr;

				if (!row[c]) {
					prow.add(std::string(colName ? colName : ""), Value());
					continue;
				}

				const unsigned long len = lengths ? lengths[c] : 0;
				const enum_field_types ftype = field ? field->type : MYSQL_TYPE_STRING;

				switch (ftype) {
					case MYSQL_TYPE_TINY: {
						if (field && (field->flags & UNSIGNED_FLAG) == 0 && field->length == 1) {
							const bool b = (row[c][0] != '0');
							prow.add(std::string(colName ? colName : ""), b);
						} else {
							long long v = 0;
							try { v = std::stoll(std::string(row[c], len)); } catch (...) { v = 0; }
							if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
								prow.add(std::string(colName ? colName : ""), static_cast<long int>(v));
							else
								prow.add(std::string(colName ? colName : ""), static_cast<int>(v));
						}
						break;
					}

					case MYSQL_TYPE_SHORT:
					case MYSQL_TYPE_LONG:
					case MYSQL_TYPE_INT24: {
						long long v = 0;
						try { v = std::stoll(std::string(row[c], len)); } catch (...) { v = 0; }
						if (v > std::numeric_limits<int>::max() || v < std::numeric_limits<int>::min())
							prow.add(std::string(colName ? colName : ""), static_cast<long int>(v));
						else
							prow.add(std::string(colName ? colName : ""), static_cast<int>(v));
						break;
					}

					case MYSQL_TYPE_LONGLONG: {
						long long v = 0;
						try { v = std::stoll(std::string(row[c], len)); } catch (...) { v = 0; }
						prow.add(std::string(colName ? colName : ""), static_cast<long int>(v));
						break;
					}

					case MYSQL_TYPE_FLOAT:
					case MYSQL_TYPE_DOUBLE:
					case MYSQL_TYPE_DECIMAL:
					case MYSQL_TYPE_NEWDECIMAL: {
						double d = 0.0;
						try { d = std::stod(std::string(row[c], len)); } catch (...) { d = 0.0; }
						prow.add(std::string(colName ? colName : ""), d);
						break;
					}

					case MYSQL_TYPE_TINY_BLOB:
					case MYSQL_TYPE_MEDIUM_BLOB:
					case MYSQL_TYPE_LONG_BLOB:
					case MYSQL_TYPE_BLOB: {
						const bool is_binary = field && field->charsetnr == 63;
						if (is_binary) {
							std::vector<std::byte> blob;
							if (len > 0) {
								blob.assign(
									reinterpret_cast<const std::byte*>(row[c]),
									reinterpret_cast<const std::byte*>(row[c]) + len
								);
							}
							prow.add(std::string(colName ? colName : ""), std::move(blob));
						} else {
							prow.add(std::string(colName ? colName : ""), std::string(row[c], len));
						}
						break;
					}

					case MYSQL_TYPE_VAR_STRING:
					case MYSQL_TYPE_STRING:
					case MYSQL_TYPE_VARCHAR:
					default: {
						prow.add(std::string(colName ? colName : ""), std::string(row[c] ? row[c] : "", len));
						break;
					}
				}
			}
			rows.add(std::move(prow));
		}

		return rows;
	}
}
