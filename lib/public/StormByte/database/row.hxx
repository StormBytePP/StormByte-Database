#pragma once

#include <StormByte/database/named_value.hxx>
#include <StormByte/iterable.hxx>

#include <unordered_map>
#include <vector>
#include <optional>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Row
	 * @brief Single result row: ordered NamedValues with lookup by column name.
	 */
	class STORMBYTE_DATABASE_PUBLIC Row: public Iterable<std::vector<NamedValue>> {
		public:
			/**
			 * Default constructor.
			 */
			Row() noexcept = default;

			/**
			 * Copy constructor.
			 * @param other Source row.
			 */
			Row(const Row& other);

			/**
			 * Move constructor.
			 */
			Row(Row&& other) noexcept = default;

			/**
			 * Destructor.
			 */
			~Row() noexcept override = default;

			/**
			 * Copy assignment.
			 * @param other Source row.
			 * @return *this
			 */
			Row& operator=(const Row& other);

			/**
			 * Move assignment.
			 */
			Row& operator=(Row&& other) noexcept = default;

			/**
			 * Access by column name (const lvalue).
			 * @param columnName Column name.
			 * @return Reference to the value.
			 * @throws ColumnNotFound if the name is absent.
			 */
			const Value& operator[](const std::string& columnName) const &;

			/**
			 * Access by column name (lvalue).
			 * @param columnName Column name.
			 * @return Reference to the value.
			 * @throws ColumnNotFound if the name is absent.
			 */
			Value& operator[](const std::string& columnName) &;

			/**
			 * Access by column name (rvalue).
			 * @param columnName Column name.
			 * @return Value (moved).
			 * @throws ColumnNotFound if the name is absent.
			 */
			Value operator[](const std::string& columnName) &&;

			using Iterable::operator[];

			/**
			 * Appends a named column.
			 * @param columnName Column name.
			 * @param value Value to store.
			 */
			inline void add(std::string&& columnName, Value&& value) {
				m_data.emplace_back(std::move(columnName), std::move(value));
				m_name_index.reset();
			}

			/**
			 * @return Number of columns.
			 */
			inline std::size_t Count() const noexcept {
				return size();
			}

		private:
			mutable std::optional<std::unordered_map<std::string, std::size_t>> m_name_index;	///< Lazy name → index map

			/**
			 * Builds m_name_index if not yet present.
			 */
			void BuildNameIndex() const;
	};
}
