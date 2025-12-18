#pragma once

#include <StormByte/database/named_value.hxx>
#include <StormByte/iterable.hxx>

#include <vector>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Row
	 * @brief Row class for databases
	 */
	class STORMBYTE_DATABASE_PUBLIC Row: public Iterable<std::vector<NamedValue>> {
		public:
			/**
			 * @brief Default Constructor
			 */
			Row() noexcept											= default;

			/**
			 * @brief Copy Constructor
			 * @param other Other Row to copy from
			 */
			Row(const Row& other)									= default;

			/**
			 * @brief Move Constructor
			 * @param other Other Row to move from
			 */
			Row(Row&& other) noexcept								= default;

			/**
			 * @brief Destructor
			 */
			~Row() noexcept override								= default;

			/**
			 * @brief Copy Assignment Operator
			 * @param other Other Row to copy from
			 * @return Reference to this Row
			 */
			Row& operator=(const Row& other)						= default;

			/**
			 * @brief Move Assignment Operator
			 * @param other Other Row to move from
			 * @return Reference to this Row
			 */
			Row& operator=(Row&& other) noexcept					= default;

			/**
			 * @brief Gets the value for the specified column name
			 * @param columnName Name of the column
			 * @return Reference to the Value for the specified column name
			 * @throw ColumnNotFound if the column name does not exist
			 */
			const Value&											operator[](const std::string& columnName) const &;

			/**
			 * @brief Gets the value for the specified column name
			 * @param columnName Name of the column
			 * @return Reference to the Value for the specified column name
			 * @throw ColumnNotFound if the column name does not exist
			 */
			Value&													operator[](const std::string& columnName) &;

			/**
			 * @brief Gets the value for the specified column name (rvalue)
			 * @param columnName Name of the column
			 * @return Value for the specified column name
			 * @throw ColumnNotFound if the column name does not exist
			 */
			Value													operator[](const std::string& columnName) &&;

			// Inherit operator[] from Iterable
			using Iterable::operator[];

			/**
			 * @brief Adds a value to the Row
			 * @param columnName Optional name of the column
			 * @param value Value to add
			 */
			inline void												add(std::string&& columnName, Value&& value) {
				m_data.emplace_back(std::move(columnName), std::move(value));
			}

			/**
			 * @brief Gets number of columns in the Row
			 * @return Number of columns in the Row
			 */
			inline std::size_t										Count() const noexcept {
				return size();
			}
	};
}
