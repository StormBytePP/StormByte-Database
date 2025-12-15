#pragma once

#include <StormByte/database/value.hxx>

#include <iterator>
#include <utility>
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
	class STORMBYTE_DATABASE_PUBLIC Row {
		public:
			/**
			 * @class Iterator
			 * @brief Iterator class for Row
			 */
			class STORMBYTE_DATABASE_PUBLIC Iterator {
				public:
					using iterator_category							= std::random_access_iterator_tag;
					using value_type								= Value;
					using difference_type							= std::ptrdiff_t;
					using pointer									= Value*;
					using reference									= Value&;

					/**
					 * @brief Dereference operator
					 * @return Reference to the current Value
					 */
					inline reference								operator*() {
						return (*m_row)[m_index];
					}

					/**
					 * @brief Arrow operator
					 * @return Pointer to the current Value
					 */
					inline pointer									operator->() {
						return &(*m_row)[m_index];
					}

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented Iterator
					 */
					inline Iterator&								operator++() {
						++m_index;
						return *this;
					}

					/**
					 * @brief Post-increment operator
					 * @return Iterator before increment
					 */
					inline Iterator									operator++(int) {
						Iterator tmp = *this;
						++(*this);
						return tmp;
					}

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented Iterator
					 */
					inline Iterator& 								operator--() noexcept {
						--m_index;
						return *this;
					}

					/**
					 * @brief Post-decrement operator
					 * @return Iterator before decrement
					 */
					inline Iterator									operator--(int) noexcept {
						Iterator tmp = *this;
						--(*this);
						return tmp;
					}

					/**
					 * @brief Addition assignment operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced Iterator
					 */
					inline Iterator& 								operator+=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) + n);
						return *this;
					}

					/**
					 * @brief Subtraction assignment operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated Iterator
					 */
					inline 	Iterator& 								operator-=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) - n);
						return *this;
					}

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New Iterator advanced by n positions
					 */
					inline Iterator 								operator+(difference_type n) const noexcept {
						Iterator tmp = *this;
						tmp += n;
						return tmp;
					}

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New Iterator retreated by n positions
					 */
					inline Iterator 								operator-(difference_type n) const noexcept {
						Iterator tmp = *this;
						tmp -= n;
						return tmp;
					}

					/**
					 * @brief Difference operator
					 * @param other Another Iterator to compare with
					 * @return Difference in positions between the two Iterators
					 */
					inline difference_type 							operator-(const Iterator& other) const noexcept {
						return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
					}

					/**
					 * @brief Equality operator
					 * @param other Another Iterator to compare with
					 * @return True if both Iterators point to the same position in the same Row
					 */
					inline bool										operator==(const Iterator& other) const noexcept {
						return m_row == other.m_row && m_index == other.m_index;
					}

					/**
					 * @brief Inequality operator
					 * @param other Another Iterator to compare with
					 * @return True if both Iterators point to different positions or different Rows
					 */
					inline bool										operator!=(const Iterator& other) const noexcept {
						return !(*this == other);
					}

				private:
					friend class Row;

					/**
					 * @brief Constructor
					 * @param row Pointer to the Row
					 * @param index Index within the Row
					 */
					Iterator(Row* row, std::size_t index) noexcept:
					m_row(row), m_index(index) {}

					Row* m_row;										///< Pointer to the Row
					std::size_t m_index;							///< Current index within the Row
			};

			class STORMBYTE_DATABASE_PUBLIC ConstIterator {
				public:
					using iterator_category							= std::random_access_iterator_tag;
					using value_type								= const Value;
					using difference_type							= std::ptrdiff_t;
					using pointer									= const Value*;
					using reference									= const Value&;

					/**
					 * @brief Dereference operator
					 * @return Reference to the current Value
					 */
					inline reference								operator*() const {
						return (*m_row)[m_index];
					}

					/**
					 * @brief Arrow operator
					 * @return Pointer to the current Value
					 */
					inline pointer									operator->() const {
						return &(*m_row)[m_index];
					}

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented ConstIterator
					 */
					inline ConstIterator&							operator++() {
						++m_index;
						return *this;
					}

					/**
					 * @brief Post-increment operator
					 * @return ConstIterator before increment
					 */
					inline ConstIterator							operator++(int) {
						ConstIterator tmp = *this;
						++(*this);
						return tmp;
					}

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented ConstIterator
					 */
					inline ConstIterator& 							operator--() noexcept {
						--m_index;
						return *this;
					}

					/**
					 * @brief Post-decrement operator
					 * @return ConstIterator before decrement
					 */
					inline ConstIterator 							operator--(int) noexcept {
						ConstIterator tmp = *this;
						--(*this);
						return tmp;
					}

					/**
					 * @brief Addition assignment operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced ConstIterator
					 */
					inline ConstIterator& 							operator+=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) + n);
						return *this;
					}

					/**
					 * @brief Subtraction assignment operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated ConstIterator
					 */
					inline ConstIterator& 							operator-=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) - n);
						return *this;
					}

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New ConstIterator advanced by n positions
					 */
					inline ConstIterator 							operator+(difference_type n) const noexcept {
						ConstIterator tmp = *this;
						tmp += n;
						return tmp;
					}

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New ConstIterator retreated by n positions
					 */
					inline ConstIterator 							operator-(difference_type n) const noexcept {
						ConstIterator tmp = *this;
						tmp -= n;
						return tmp;
					}

					/**
					 * @brief Difference operator
					 * @param other Another ConstIterator to compare with
					 * @return Difference in positions between the two ConstIterators
					 */
					inline difference_type 							operator-(const ConstIterator& other) const noexcept {
						return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
					}

					/**
					 * Random access by offset from const iterator
					 */
					inline reference 								operator[](difference_type n) const {
						return (*m_row)[m_index + static_cast<std::size_t>(n)];
					}

					/**
					 * @brief Equality operator
					 * @param other Another ConstIterator to compare with
					 * @return True if both ConstIterators point to the same position in the same Row
					 */
					inline bool										operator==(const ConstIterator& other) const noexcept {
						return m_row == other.m_row && m_index == other.m_index;
					}

					/**
					 * @brief Inequality operator
					 * @param other Another ConstIterator to compare with
					 * @return True if both ConstIterators do not point to the same position in the same Row
					 */
					inline bool										operator!=(const ConstIterator& other) const noexcept {
						return !(*this == other);
					}

				private:
					friend class Row;

					/**
					 * @brief Constructor
					 * @param row Pointer to the Row
					 * @param index Index within the Row
					 */
					ConstIterator(const Row* row, std::size_t index) noexcept:
					m_row(row), m_index(index) {}

					const Row* m_row;								///< Pointer to the Row
					std::size_t m_index;							///< Current index within the Row
			};

			using iterator = Iterator;
			using const_iterator = ConstIterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
			~Row() noexcept											= default;

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
			 * @brief Gets the number of columns in the Row
			 * @param index Index of the value to retrieve
			 * @return Number of columns
			 * @throw OutOfBounds if index is out of bounds
			 */
			const Value&											operator[](std::size_t index) const &;

			/**
			 * @brief Gets the value at the specified index
			 * @param index Index of the value to retrieve
			 * @return Reference to the Value at the specified index
			 * @throw OutOfBounds if index is out of bounds
			 */
			Value&													operator[](std::size_t index) &;

			/**
			 * @brief Gets the value at the specified index (rvalue)
			 * @param index Index of the value to retrieve
			 * @return Value at the specified index
			 * @throw OutOfBounds if index is out of bounds
			 */
			Value													operator[](std::size_t index) &&;

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

			/**
			 * @brief Adds a value to the Row
			 * @param columnName Optional name of the column
			 * @param value Value to add
			 */
			inline void												Add(std::string&& columnName, Value&& value) {
				m_values.emplace_back(std::move(columnName), std::move(value));
			}

			/**
			 * @brief Gets begin iterator
			 * @return Iterator to the first element
			 */
			inline iterator 										begin() noexcept {
				return Iterator(this, 0);
			}

			/**
			 * @brief Gets end iterator
			 * @return Iterator to past the last element
			 */
			inline iterator 										end() noexcept {
				return Iterator(this, m_values.size());
			}

			/**
			 * @brief Gets const begin iterator
			 * @return ConstIterator to the first element
			 */
			inline const_iterator									begin() const noexcept {
				return ConstIterator(this, 0);
			}

			/**
			 * @brief Gets const end iterator
			 * @return ConstIterator to past the last element
			 */
			inline const_iterator									end() const noexcept {
				return ConstIterator(this, m_values.size());
			}

			/**
			 * @brief Gets const begin iterator
			 * @return ConstIterator to the first element
			 */
			inline const_iterator									cbegin() const noexcept {
				return ConstIterator(this, 0);
			}

			/**
			 * @brief Gets const end iterator
			 * @return ConstIterator to past the last element
			 */
			inline const_iterator									cend() const noexcept {
				return ConstIterator(this, m_values.size());
			}

			/**
			 * @brief Gets reverse begin iterator
			 * @return Reverse iterator to the last element
			 */
			inline reverse_iterator 								rbegin() noexcept {
				return reverse_iterator(end());
			}

			/**
			 * @brief Gets reverse end iterator
			 * @return Reverse iterator to before the first element
			 */
			inline reverse_iterator 								rend() noexcept {
				return reverse_iterator(begin());
			}

			/**
			 * @brief Gets const reverse begin iterator
			 * @return Const reverse iterator to the last element
			 */
			inline const_reverse_iterator 							rbegin() const noexcept {
				return const_reverse_iterator(end());
			}

			/**
			 * @brief Gets const reverse end iterator
			 * @return Const reverse iterator to before the first element
			 */
			inline const_reverse_iterator 							rend() const noexcept {
				return const_reverse_iterator(begin());
			}

			/**
			 * @brief Gets number of columns in the Row
			 * @return Number of columns in the Row
			 */
			inline std::size_t										Count() const noexcept {
				return m_values.size();
			}

		private:
			std::vector<std::pair<std::string, Value>> m_values;	///< Internal storage of values
	};
}
