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

#include <StormByte/exception.hxx>
#include <StormByte/database/visibility.h>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Exception
	 * @brief Base class for Database exceptions.
	 */
	class STORMBYTE_DATABASE_PUBLIC Exception: public StormByte::Exception {
		public:
			/**
			 * Construct with component prefix and format string.
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
			 * Destructor.
			 */
			virtual ~Exception() noexcept override = default;
	};

	/**
	 * @class ConnectionError
	 * @brief Exception thrown on connection failures.
	 */
	class STORMBYTE_DATABASE_PUBLIC ConnectionError final: public Exception {
		public:
			/**
			 * @param error Error description.
			 */
			ConnectionError(const std::string& error):
			Exception("Connection: ", error) {}

			using Exception::Exception;
	};

	/**
	 * @class WrongValueType
	 * @brief Exception thrown when a Value is accessed with the wrong type.
	 */
	class STORMBYTE_DATABASE_PUBLIC WrongValueType final: public Exception {
		public:
			/**
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
	 * @brief Exception when a column name is not present in a Row.
	 */
	class STORMBYTE_DATABASE_PUBLIC ColumnNotFound: public Exception {
		public:
			/**
			 * @param column Column name that was not found.
			 */
			template <typename... Args>
			ColumnNotFound(const std::string& column):
			Exception("ColumnNotFound: ", "Column '{}' not found", column) {}

			using Exception::Exception;
	};

	/**
	 * @class OutOfBounds
	 * @brief Exception when a column index is out of range.
	 */
	class STORMBYTE_DATABASE_PUBLIC OutOfBounds: public Exception {
		public:
			/**
			 * @param pos Requested index.
			 * @param size Container size.
			 */
			OutOfBounds(int pos, std::size_t size):
			Exception("OutOfBounds: ", "Position {} is out of bounds for size {}", pos, size) {}

			using Exception::Exception;
	};

	/**
	 * @class QueryException
	 * @brief Base for query-related errors.
	 */
	class STORMBYTE_DATABASE_PUBLIC QueryException: public Exception {
		public:
			/**
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
	 * @brief Exception when a prepared statement name is not registered.
	 */
	class STORMBYTE_DATABASE_PUBLIC UnknownSTMT: public QueryException {
		public:
			/**
			 * @param name Statement name.
			 */
			UnknownSTMT(const std::string& name):
			QueryException("PreparedSTMT: ", "Statement '{}' not found", name) {}

			using QueryException::QueryException;
	};

	/**
	 * @class ExecuteError
	 * @brief Exception when query or statement execution fails.
	 */
	class STORMBYTE_DATABASE_PUBLIC ExecuteError: public QueryException {
		public:
			/**
			 * @param error Backend error message.
			 */
			ExecuteError(const std::string& error):
			QueryException("Execute: ", "Error executing query: {}", error) {}

			using QueryException::QueryException;
	};
}
