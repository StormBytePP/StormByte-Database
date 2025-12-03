#pragma once

#include <string>
#include <variant>

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	using Value = std::variant<std::nullptr_t, int, int64_t, double, std::string>;		///< Shortcut alias for value
	using Row = StormByte::Database::Row<Value>;										///< Shortcut alias for row
}