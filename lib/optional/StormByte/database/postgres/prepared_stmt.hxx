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

struct pg_conn;

/**
 * @brief PostgreSQL backend of the Database module.
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class PreparedSTMT
	 * @brief PostgreSQL prepared statement (PQexecPrepared).
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class Postgres;
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
		~PreparedSTMT() noexcept override = default;

		/**
		 * @brief Copy assignment (deleted).
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * @brief Move assignment.
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		struct pg_conn* m_conn;							///< Connection handle
		std::string m_stmt_name;						///< Server-side statement name

		std::vector<const char*> m_param_values;		///< Bind value pointers
		std::vector<int> m_param_lengths;				///< Bind lengths (blobs)
		std::vector<int> m_param_formats;				///< 0 = text, 1 = binary
		std::vector<std::string> m_string_storage;		///< Owns text/numeric string params
		std::vector<std::vector<char>> m_blob_storage;	///< Owns blob params

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
		 * @brief Store a bound value at @p index.
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * @brief Execute via PQexecPrepared.
		 * @return Result rows or an error.
		 */
		ExpectedRows DoExecute() override;

		/**
		 * @brief Clear all bind storage.
		 */
		void Reset() noexcept override;
	};
}
