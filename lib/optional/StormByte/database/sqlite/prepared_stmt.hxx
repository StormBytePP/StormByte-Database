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

class sqlite3_stmt;

/**
 * @brief SQLite backend of the Database module.
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class PreparedSTMT
	 * @brief SQLite prepared statement.
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class SQLite3;
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
		sqlite3_stmt* m_stmt;	///< SQLite statement handle

		/**
		 * @brief Construct from copies.
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger);

		/**
		 * @brief Construct from moved strings.
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept;

		/**
		 * @brief Bind at @p index (0-based; SQLite is 1-based internally).
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * @brief Step the statement and build Rows.
		 * @return Result rows or an error.
		 */
		ExpectedRows DoExecute() override;

		/**
		 * @brief Clear bindings and reset the statement.
		 */
		void Reset() noexcept override;
	};
}
