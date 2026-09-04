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

#include <StormByte/database/visibility.h>
#include <StormByte/exception.hxx>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class Exception
	 * @brief Base exception for the Database module.
	 */
	class STORMBYTE_DATABASE_PUBLIC Exception: public StormByte::Exception {
		public:
			/**
			 * @brief Construct with a component prefix and a format string.
			 * @tparam Args Format argument types.
			 * @param component Subsystem name.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			Exception(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			StormByte::Exception("Database::" + component, fmt, std::forward<Args>(args)...) {}

			using StormByte::Exception::Exception;

			/**
			 * @brief Destructor.
			 */
			virtual ~Exception() noexcept override = default;
	};

	/**
	 * @class ConnectionError
	 * @brief Connection failed.
	 */
	class STORMBYTE_DATABASE_PUBLIC ConnectionError final: public Exception {
		public:
			/**
			 * @brief Construct from a backend message.
			 * @param error Error text.
			 */
			ConnectionError(const std::string& error):
			Exception("Connection: ", error) {}

			using Exception::Exception;
	};

	/**
	 * @class WrongValueType
	 * @brief Value accessed as the wrong type.
	 */
	class STORMBYTE_DATABASE_PUBLIC WrongValueType final: public Exception {
		public:
			/**
			 * @brief Construct with a format string.
			 * @tparam Args Format argument types.
			 * @param component Context label.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			WrongValueType(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			Exception("WrongValueType: ", fmt, std::forward<Args>(args)...) {}

			using Exception::Exception;
	};

	/**
	 * @class ColumnNotFound
	 * @brief Column name missing from a Row.
	 */
	class STORMBYTE_DATABASE_PUBLIC ColumnNotFound: public Exception {
		public:
			/**
			 * @brief Construct from the missing name.
			 * @param column Column name.
			 */
			template <typename... Args>
			ColumnNotFound(const std::string& column):
			Exception("ColumnNotFound: ", "Column '{}' not found", column) {}

			using Exception::Exception;
	};

	/**
	 * @class OutOfBounds
	 * @brief Column index out of range.
	 */
	class STORMBYTE_DATABASE_PUBLIC OutOfBounds: public Exception {
		public:
			/**
			 * @brief Construct from index and size.
			 * @param pos Requested index.
			 * @param size Container size.
			 */
			OutOfBounds(int pos, std::size_t size):
			Exception("OutOfBounds: ", "Position {} is out of bounds for size {}", pos, size) {}

			using Exception::Exception;
	};

	/**
	 * @class QueryException
	 * @brief Base for query errors.
	 */
	class STORMBYTE_DATABASE_PUBLIC QueryException: public Exception {
		public:
			/**
			 * @brief Construct with a query subsystem prefix.
			 * @tparam Args Format argument types.
			 * @param component Subsystem name.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			QueryException(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			Exception("Query::" + component, fmt, std::forward<Args>(args)...) {}

			using Exception::Exception;
	};

	/**
	 * @class UnknownSTMT
	 * @brief Prepared statement name is not registered.
	 */
	class STORMBYTE_DATABASE_PUBLIC UnknownSTMT: public QueryException {
		public:
			/**
			 * @brief Construct from the statement name.
			 * @param name Statement name.
			 */
			UnknownSTMT(const std::string& name):
			QueryException("PreparedSTMT: ", "Statement '{}' not found", name) {}

			using QueryException::QueryException;
	};

	/**
	 * @class ExecuteError
	 * @brief Query or statement execution failed.
	 */
	class STORMBYTE_DATABASE_PUBLIC ExecuteError: public QueryException {
		public:
			/**
			 * @brief Construct from a backend message.
			 * @param error Error text.
			 */
			ExecuteError(const std::string& error):
			QueryException("Execute: ", "Error executing query: {}", error) {}

			using QueryException::QueryException;
	};
}
