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

#include <sqlite3.h>
#include <limits>

/**
 * @namespace SQLite
 * @brief SQLite backend for StormByte::Database.
 */
namespace StormByte::Database::SQLite {
	/**
	 * Steps through an SQLite statement and builds Rows.
	 * @param stmt Prepared statement (must not be null).
	 * @return Result rows or a QueryException.
	 *
	 * @note Inline and public-visible on Windows even though it lives under private/.
	 */
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
							row.add(std::string(colName ? colName : ""), static_cast<long int>(v));
						else
							row.add(std::string(colName ? colName : ""), static_cast<int>(v));
						break;
					}
					case SQLITE_FLOAT:
						row.add(std::string(colName ? colName : ""), sqlite3_column_double(stmt, i));
						break;
					case SQLITE_TEXT: {
						const unsigned char* text = sqlite3_column_text(stmt, i);
						row.add(std::string(colName ? colName : ""), std::string(reinterpret_cast<const char*>(text ? text : (const unsigned char*)"")));
						break;
					}
					case SQLITE_BLOB: {
						const std::byte* blobData = reinterpret_cast<const std::byte*>(sqlite3_column_blob(stmt, i));
						int blobSize = sqlite3_column_bytes(stmt, i);
						std::vector<std::byte> blobVec;
						if (blobData && blobSize > 0)
							blobVec.assign(blobData, blobData + blobSize);
						row.add(std::string(colName ? colName : ""), std::move(blobVec));
						break;
					}
					case SQLITE_NULL:
					default:
						row.add(std::string(colName ? colName : ""), Value());
						break;
				}
			}
			rows.add(std::move(row));
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
