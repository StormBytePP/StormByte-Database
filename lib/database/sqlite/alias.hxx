#pragma once

#include <string>

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	using Value = StormByte::Util::Templates::VariadicValue<std::nullptr_t, int, int64_t, double, std::string>;	///< Shortcut alias for value
	using Row = StormByte::Database::Row<Value>;																///< Shortcut alias for row
}