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

#include <format>
#include <string>
#include <utility>

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
			 * @brief Construct with an unformatted message.
			 * @param message Message text.
			 */
			explicit Exception(const std::string& message):
				StormByte::Exception(StormByte::Component{"Database"}, "{}", message) {}

			/**
			 * @brief Construct with a moved unformatted message.
			 * @param message Message text.
			 */
			explicit Exception(std::string&& message):
				StormByte::Exception(StormByte::Component{"Database"}, "{}", std::move(message)) {}

			/**
			 * @brief Construct with a formatted message.
			 * @tparam Args Format argument types.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			Exception(std::format_string<Args...> fmt, Args&&... args):
				StormByte::Exception(StormByte::Component{"Database"}, fmt, std::forward<Args>(args)...) {}

			/**
			 * @brief Destructor.
			 */
			virtual ~Exception() noexcept override = default;

		protected:
			/**
			 * @brief Construct with a qualified database component and formatted message.
			 * @tparam Args Format argument types.
			 * @param component Qualified component name.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			Exception(StormByte::Component component, std::format_string<Args...> fmt, Args&&... args):
				StormByte::Exception(component, fmt, std::forward<Args>(args)...) {}
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
				Exception(StormByte::Component{"Database::Connection"}, "{}", error) {}

			/**
			 * @brief Construct from a moved backend message.
			 * @param error Error text.
			 */
			ConnectionError(std::string&& error):
				Exception(StormByte::Component{"Database::Connection"}, "{}", std::move(error)) {}
	};

	/**
	 * @class WrongValueType
	 * @brief Value accessed as the wrong type.
	 */
	class STORMBYTE_DATABASE_PUBLIC WrongValueType final: public Exception {
		public:
			/**
			 * @brief Construct from an error message.
			 * @param error Error text.
			 */
			WrongValueType(const std::string& error):
				Exception(StormByte::Component{"Database::WrongValueType"}, "{}", error) {}

			/**
			 * @brief Construct from a moved error message.
			 * @param error Error text.
			 */
			WrongValueType(std::string&& error):
				Exception(StormByte::Component{"Database::WrongValueType"}, "{}", std::move(error)) {}

			/**
			 * @brief Construct with a format string.
			 * @tparam Args Format argument types.
			 * @param component Context label.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			WrongValueType(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
				Exception(StormByte::Component{std::string("Database::WrongValueType::") + component}, fmt, std::forward<Args>(args)...) {}

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
			ColumnNotFound(const std::string& column):
				Exception(StormByte::Component{"Database::ColumnNotFound"}, "Column '{}' not found", column) {}

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
				Exception(StormByte::Component{"Database::OutOfBounds"}, "Position {} is out of bounds for size {}", pos, size) {}

	};

	/**
	 * @class QueryException
	 * @brief Base for query errors.
	 */
	class STORMBYTE_DATABASE_PUBLIC QueryException: public Exception {
		public:
			/**
			 * @brief Construct from an error message.
			 * @param error Error text.
			 */
			QueryException(const std::string& error):
				Exception(StormByte::Component{"Database::Query"}, "{}", error) {}

			/**
			 * @brief Construct from a moved error message.
			 * @param error Error text.
			 */
			QueryException(std::string&& error):
				Exception(StormByte::Component{"Database::Query"}, "{}", std::move(error)) {}

			/**
			 * @brief Construct with a query subsystem prefix.
			 * @tparam Args Format argument types.
			 * @param component Subsystem name.
			 * @param fmt Format string.
			 * @param args Format arguments.
			 */
			template <typename... Args>
			QueryException(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
				Exception(StormByte::Component{std::string("Database::Query::") + component}, fmt, std::forward<Args>(args)...) {}

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
				QueryException("PreparedSTMT", "Statement '{}' not found", name) {}

			/**
			 * @brief Construct from a moved statement name.
			 * @param name Statement name.
			 */
			UnknownSTMT(std::string&& name):
				QueryException("PreparedSTMT", "Statement '{}' not found", name) {}
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
				QueryException("Execute", "Error executing query: {}", error) {}

			/**
			 * @brief Construct from a moved backend message.
			 * @param error Error text.
			 */
			ExecuteError(std::string&& error):
				QueryException("Execute", "Error executing query: {}", std::move(error)) {}
	};
}
