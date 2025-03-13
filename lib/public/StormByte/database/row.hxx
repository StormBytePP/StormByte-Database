#pragma once

#include <StormByte/database/exception.hxx>
#include <StormByte/util/templates/variadic_value.hxx>
#include <StormByte/util/templates/iterator.hxx>

#include <algorithm>
#include <memory>
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
	template<class Value> class STORMBYTE_DATABASE_PUBLIC Row {
		using NamedValue = std::pair<std::string, std::shared_ptr<Value>>;		///< Shortcut alias for pair
		using Storage = std::vector<NamedValue>;								///< Shortcut alias for internal storage
		public:
			using Iterator = Util::Templates::Iterator<Storage>;				///< Iterator for Row
			using ConstIterator = Util::Templates::ConstIterator<Storage>;		///< ConstIterator for Row

			/**
			 * Constructor
			 */
			Row() noexcept 										= default;

			/**
			 * Copy Constructor
			 */
			Row(const Row&)										= default;

			/**
			 * Move Constructor
			 */
			Row(Row&&) noexcept									= default;

			/**
			 * Assignment operator
			 */
			Row& operator=(const Row&)							= default;

			/**
			 * Move operator
			 */
			Row& operator=(Row&&) noexcept						= default;

			/**
			 * Destructor
			 */
			virtual ~Row() noexcept								= default;

			/**
			 * Gets value for column by name
			 * @param columnName column name
			 * @throw ColumnNotFound if column is not found
			 * @return Value
			 */
			Value& 												operator[](const std::string& columnName) {
				return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnName));
			}

			/**
			 * Gets value for column by name
			 * @param columnName column name
			 * @throw ColumnNotFound if column is not found
			 * @return Value
			 */
			const Value& 										operator[](const std::string& columnName) const {
				auto it = std::find_if(m_values.begin(), m_values.end(), [&columnName](const NamedValue& item ) { return item.first == columnName; });
				if (it == m_values.end())
					throw ColumnNotFound(columnName);
				return *(it->second);
			}

			/**
			 * Gets value for column by index
			 * @param columnIndex column index
			 * @throw OutOfBounds if index is out of bounds
			 * @return Value
			 */
			Value& 												operator[](const size_t& columnIndex) {
				return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnIndex));
			}

			/**
			 * Gets value for column by index
			 * @param columnIndex column index
			 * @throw OutOfBounds if index is out of bounds
			 * @return Value
			 */
			const Value& 										operator[](const size_t& columnIndex) const {
				if (columnIndex >= m_values.size())
					throw OutOfBounds(columnIndex);
				return *m_values[columnIndex].second;
			}

			/**
			 * Checks if result is empty
			 */
			constexpr											operator bool() const noexcept {
				return m_values.size() > 0;
			}

			/**
			 * Gets an iterator pointing to the first element
			 * @return Iterator
			 */
			Iterator 											Begin() noexcept {
				return Iterator::Begin(m_values);
			}

			/**
			 * Gets a ConstIterator pointing to the first element
			 * @return ConstIterator
			 */
			ConstIterator 										Begin() const noexcept {
				return ConstIterator::Begin(m_values);
			}

			/**
			 * Gets an iterator pointing to past last element
			 * @return Iterator
			 */
			Iterator 											End() noexcept {
				return Iterator::End(m_values);
			}

			/**
			 * Gets a ConstIterator pointing to past last element
			 * @return ConstIterator
			 */
			ConstIterator 										End() const noexcept {
				return ConstIterator::End(m_values);
			}

			/**
			 * Adds a value to the row
			 * @param columnName column name
			 * @param value value
			 */
			void												Add(const std::string& columnName, std::unique_ptr<Value>&& value) {
				m_values.push_back({columnName, std::move(value)});
			}

			/**
			 * Gets the number of columns
			 * @return number of columns
			 */
			constexpr size_t									Columns() const noexcept {
				return m_values.size();
			}

		protected:
			/**
			 * Internal storage
			 */
			Storage m_values; //< Internal storage
	};
}