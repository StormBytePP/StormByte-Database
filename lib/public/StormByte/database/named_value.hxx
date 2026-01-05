#pragma once

#include <StormByte/database/value.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class NamedValue
	 * @brief NamedValue class for databases
	 */
	class STORMBYTE_DATABASE_PUBLIC NamedValue: public Value {
		public:
			/**
			 * @brief Constructor
			 * @param name Name of the value
			 * @param value Value to store
			 */
			NamedValue(const std::string& name, const Value& value) noexcept:
			Value(value), m_name(name) {}

			/**
			 * @brief Constructor
			 * @param name Name of the value
			 * @param value Value to store
			 */
			NamedValue(std::string&& name, Value&& value) noexcept:
			Value(std::move(value)), m_name(std::move(name)) {}

			/**
			 * @brief Copy Constructor
			 * @param other Other NamedValue to copy from
			 */
			NamedValue(const NamedValue& other)						= default;

			/**
			 * @brief Move Constructor
			 * @param other Other NamedValue to move from
			 */
			NamedValue(NamedValue&& other) noexcept					= default;

			/**
			 * @brief Destructor
			 */
			~NamedValue() noexcept override							= default;

			/**
			 * @brief Assignment operator
			 * @param other Other NamedValue to copy from
			 * @return Reference to this NamedValue
			 */
			NamedValue& operator=(const NamedValue& other)			= default;

			/**
			 * @brief Move operator
			 * @param other Other NamedValue to move from
			 * @return Reference to this NamedValue
			 */
			NamedValue& operator=(NamedValue&& other) noexcept		= default;

			/**
			 * @brief Equality operator
			 * @param other Other NamedValue to compare with
			 * @return True if the NamedValues are equal, false otherwise
			 */
			inline bool 											operator==(const NamedValue& other) const noexcept {
				return m_name == other.m_name && Value::operator==(other);
			}

			/**
			 * @brief Inequality operator
			 * @param other Other NamedValue to compare with
			 * @return True if the NamedValues are not equal, false otherwise
			 */
			inline bool 											operator!=(const NamedValue& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @brief Gets the name of the value
			 * @return Name of the value
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

		private:
			std::string m_name;										///< Name of the value
	};
}