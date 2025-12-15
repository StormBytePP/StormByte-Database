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
	 * @brief Base class for Database exceptions
	 */
	class STORMBYTE_DATABASE_PUBLIC Exception: public StormByte::Exception {
		public:
			template <typename... Args>
			Exception(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			StormByte::Exception("Database::" + component, fmt, std::forward<Args>(args)...) {}

			using StormByte::Exception::Exception;

			/**
			 * Destructor
			 */
			virtual ~Exception() noexcept override = default;
	};

	/**
	 * @class WrongValueType
	 * @brief Exception thrown when value type requested is not correct
	 */
	class STORMBYTE_DATABASE_PUBLIC ConnectionError final: public Exception {
		public:
			ConnectionError(const std::string& error):
			Exception("Connection: ", error) {}
			
			using Exception::Exception;
	};

	/**
	 * @class WrongValueType
	 * @brief Exception thrown when value type requested is not correct
	 */
	class STORMBYTE_DATABASE_PUBLIC WrongValueType final: public Exception {
		public:
			template <typename... Args>
			WrongValueType(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			Exception("WrongValueType: ", fmt, std::forward<Args>(args)...) {}

			using Exception::Exception;
	};

	/**
	 * @class ColumnNotFound
	 * @brief Exception when accessing a not found column
	 */
	class STORMBYTE_DATABASE_PUBLIC ColumnNotFound: public Exception {
		public:
			template <typename... Args>
			ColumnNotFound(const std::string& collumn):
			Exception("ColumnNotFound: ", "Column '{}' not found", collumn) {}

			using Exception::Exception;
	};

	/**
	 * @class OutOfBounds
	 * @brief Exception when accessing an out of bounds column
	 */
	class STORMBYTE_DATABASE_PUBLIC OutOfBounds: public Exception {
		public:
			OutOfBounds(int pos, std::size_t size):
			Exception("OutOfBounds: ", "Position {} is out of bounds for size {}", pos, size) {}
			
			using Exception::Exception;
	};

	/**
	 * @class QueryException
	 * @brief Exception related to query execution
	 */
	class STORMBYTE_DATABASE_PUBLIC QueryException: public Exception {
		public:
			template <typename... Args>
			QueryException(const std::string& component, std::format_string<Args...> fmt, Args&&... args):
			Exception("Query::" + component, fmt, std::forward<Args>(args)...) {}
			
			using Exception::Exception;
	};

	/**
	 * @class UnknownSTMT
	 * @brief Exception when accessing an unknown prepared statement
	 */
	class STORMBYTE_DATABASE_PUBLIC UnknownSTMT: public QueryException {
		public:
			UnknownSTMT(const std::string& name):
			QueryException("PreparedSTMT: ", "Statement '{}' not found", name) {}
			
			using QueryException::QueryException;
	};

	/**
	 * @class ExecuteError
	 * @brief Exception when executing a query
	 */
	class STORMBYTE_DATABASE_PUBLIC ExecuteError: public QueryException {
		public:
			ExecuteError(const std::string& error):
			QueryException("Execute: ", "Error executing query: {}", error) {}
			
			using QueryException::QueryException;
	};
}