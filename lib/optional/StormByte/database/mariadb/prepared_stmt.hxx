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

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

#include <memory>
#include <string>
#include <vector>

struct st_mysql;
struct st_mysql_stmt;

/**
 * @brief MariaDB backend of the Database module.
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
		 * @brief Copy constructor (deleted).
		 */
		PreparedSTMT(const PreparedSTMT& other) = delete;

		/**
		 * @brief Move constructor.
		 */
		PreparedSTMT(PreparedSTMT&& other) noexcept = default;

		/**
		 * @brief Destructor.
		 */
		~PreparedSTMT() noexcept override;

		/**
		 * @brief Copy assignment (deleted).
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * @brief Move assignment.
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		struct st_mysql* m_conn;							///< Connection handle
		struct st_mysql_stmt* m_stmt;						///< Statement handle
		std::vector<StormByte::Database::Value> m_params;	///< Bound parameters

		/**
		 * @brief Construct from copies.
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param conn Connection handle.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(const std::string& name, const std::string& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger);

		/**
		 * @brief Construct from moved strings.
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param conn Connection handle.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(std::string&& name, std::string&& query, struct st_mysql* conn, std::shared_ptr<Logger::Log> logger) noexcept;

		/**
		 * @brief Grow @p params so @p index is valid.
		 * @param params Parameter vector.
		 * @param index Index to ensure.
		 */
		static void EnsureParamSize(std::vector<StormByte::Database::Value>& params, int index) noexcept;

		/**
		 * @brief Store a bound value at @p index.
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * @brief Execute and fetch rows.
		 * @return Result rows or an error.
		 */
		StormByte::Database::ExpectedRows DoExecute() override;

		/**
		 * @brief Clear parameters and reset the statement.
		 */
		void Reset() noexcept override;
	};
}
