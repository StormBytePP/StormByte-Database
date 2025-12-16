#pragma once

#include <StormByte/expected.hxx>
#include <StormByte/database/exception.hxx>

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	class Rows; 											///< Forward declaration of Rows class
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
	>;														///< Internal value type alias
	using ExpectedRows = Expected<Rows, QueryException>;	///< Expected Rows type alias
}