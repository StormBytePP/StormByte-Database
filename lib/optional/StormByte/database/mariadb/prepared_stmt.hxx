/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

#include <vector>
#include <string>
#include <memory>

struct st_mysql;
struct st_mysql_stmt;

/**
 * @namespace MariaDB
 * @brief MariaDB backend for StormByte::Database.
 */
namespace StormByte::Database::MariaDB {
	class MariaDB;

	/**
	 * @class PreparedSTMT
	 * @brief MariaDB prepared statement.
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class ::StormByte::Database::MariaDB::MariaDB;
	public:
		/**
		 * Copy constructor (deleted).
		 */
		PreparedSTMT(const PreparedSTMT& other) = delete;

		/**
		 * Move constructor.
		 */
		PreparedSTMT(PreparedSTMT&& other) noexcept = default;

		/**
		 * Destructor.
		 */
		~PreparedSTMT() noexcept override;

		/**
		 * Copy assignment (deleted).
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * Move assignment.
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		struct st_mysql* m_conn;						///< Connection handle
		struct st_mysql_stmt* m_stmt;					///< Statement handle
		std::vector<StormByte::Database::Value> m_params;	///< Bound parameters

		/**
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param conn Connection handle.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(const std::string& name, const std::string& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger);

		/**
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param conn Connection handle.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(std::string&& name, std::string&& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger) noexcept;

		/**
		 * Grows @p params so that @p index is valid.
		 * @param params Parameter vector.
		 * @param index Index to ensure.
		 */
		static void EnsureParamSize(std::vector<StormByte::Database::Value>& params, int index) noexcept;

		/**
		 * Stores a bound value at @p index.
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * Executes the statement and fetches result rows.
		 * @return Result rows or an error.
		 */
		StormByte::Database::ExpectedRows DoExecute() override;

		/**
		 * Clears parameters and resets the statement.
		 */
		void Reset() noexcept override;
	};
}
