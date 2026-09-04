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

#include <StormByte/database/exception.hxx>
#include <StormByte/expected.hxx>

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	class Rows;

	/**
	 * @typedef ValuesVariant
	 * @brief Column alternatives. `std::monostate` is SQL NULL.
	 */
	using ValuesVariant = std::variant<
		std::monostate,
		int,
		unsigned int,
		long int,
		unsigned long int,
		double,
		std::string,
		bool,
		std::vector<std::byte>
	>;

	/**
	 * @typedef ExpectedRows
	 * @brief Query result: Rows or QueryException.
	 */
	using ExpectedRows = Expected<Rows, QueryException>;

	/**
	 * @enum SslMode
	 * @brief TLS policy for MariaDB / PostgreSQL. SQLite ignores it.
	 */
	enum class SslMode {
		Default,	///< Driver default
		Disable,	///< No TLS
		Prefer,		///< TLS if the server offers it
		Require		///< Fail if TLS cannot be used
	};

	/**
	 * @enum IsolationLevel
	 * @brief Isolation for BeginTransaction(). Mapping is backend-specific.
	 */
	enum class IsolationLevel {
		Default,			///< Backend default
		ReadUncommitted,	///< Dirty reads where supported
		ReadCommitted,		///< Committed data only
		RepeatableRead,		///< Stable reads in the transaction
		Serializable		///< Full serializability where supported
	};
}
