#pragma once

#include <database/visibility.h>

/**
 * @namespace Database::SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @enum Type
	 * @brief Type of a value
	 */
	enum class STORMBYTE_DATABASE_PUBLIC Type: unsigned short {
		Integer = 0,	///< Integer type
		Double,			///< Double type
		Bool,			///< Bool type
		String,			///< String type
		Null			///< Null type
	};
	/**
	 * Gets Type string
	 * @return string
	 */
	constexpr STORMBYTE_DATABASE_PUBLIC const char* GetTypeAsString(const Type& t) noexcept {
		switch(t) {
			case Type::Integer:	return "Integer";
			case Type::Double:	return "Double";
			case Type::Bool:	return "Bool";
			case Type::String:	return "String";
			case Type::Null:	return "Null";
			default:			return "Unknown";
		}
	}
}
