#pragma once

#include <StormByte/database/value.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class NamedValue
	 * @brief Value with an associated column name.
	 */
	class STORMBYTE_DATABASE_PUBLIC NamedValue: public Value {
		public:
			/**
			 * @param name Column name.
			 * @param value Value to store.
			 */
			NamedValue(const std::string& name, const Value& value) noexcept:
			Value(value), m_name(name) {}

			/**
			 * @param name Column name.
			 * @param value Value to store.
			 */
			NamedValue(std::string&& name, Value&& value) noexcept:
			Value(std::move(value)), m_name(std::move(name)) {}

			/**
			 * Copy constructor.
			 */
			NamedValue(const NamedValue& other) = default;

			/**
			 * Move constructor.
			 */
			NamedValue(NamedValue&& other) noexcept = default;

			/**
			 * Destructor.
			 */
			~NamedValue() noexcept override = default;

			/**
			 * Copy assignment.
			 */
			NamedValue& operator=(const NamedValue& other) = default;

			/**
			 * Move assignment.
			 */
			NamedValue& operator=(NamedValue&& other) noexcept = default;

			/**
			 * Equality (name and value).
			 * @param other Other NamedValue.
			 * @return true if equal.
			 */
			inline bool operator==(const NamedValue& other) const noexcept {
				return m_name == other.m_name && Value::operator==(other);
			}

			/**
			 * Inequality.
			 * @param other Other NamedValue.
			 * @return true if not equal.
			 */
			inline bool operator!=(const NamedValue& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @return Column name.
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

		private:
			std::string m_name;	///< Column name
	};
}
