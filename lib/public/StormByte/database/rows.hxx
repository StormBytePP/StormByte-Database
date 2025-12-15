#pragma once

#include <StormByte/database/row.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Rows
	 * @brief Rows class for databases
	 */
	class STORMBYTE_DATABASE_PUBLIC Rows {
		public:
			/**
			 * @class Iterator
			 * @brief Iterator class for Rows
			 */
			class STORMBYTE_DATABASE_PUBLIC Iterator {
				public:
					using iterator_category						= std::random_access_iterator_tag;
					using value_type							= Row;
					using difference_type						= std::ptrdiff_t;
					using pointer								= Row*;
					using reference								= Row&;

					/**
					 * @brief Dereference operator
					 * @return Reference to the current Value
					 */
					inline reference							operator*() {
						return (*m_rows)[m_index];
					}

					/**
					 * @brief ArRows operator
					 * @return Pointer to the current Value
					 */
					inline pointer								operator->() {
						return &(*m_rows)[m_index];
					}

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented Iterator
					 */
					inline Iterator&							operator++() {
						++m_index;
						return *this;
					}

					/**
					 * @brief Post-increment operator
					 * @return Iterator before increment
					 */
					inline Iterator								operator++(int) {
						Iterator tmp = *this;
						++(*this);
						return tmp;
					}

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented Iterator
					 */
					inline Iterator& 							operator--() noexcept {
						--m_index;
						return *this;
					}

					/**
					 * @brief Post-decrement operator
					 * @return Iterator before decrement
					 */
					inline Iterator								operator--(int) noexcept {
						Iterator tmp = *this;
						--(*this);
						return tmp;
					}

					/**
					 * @brief Addition assignment operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced Iterator
					 */
					inline Iterator& 							operator+=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) + n);
						return *this;
					}

					/**
					 * @brief Subtraction assignment operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated Iterator
					 */
					inline 	Iterator& 							operator-=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) - n);
						return *this;
					}

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New Iterator advanced by n positions
					 */
					inline Iterator 							operator+(difference_type n) const noexcept {
						Iterator tmp = *this;
						tmp += n;
						return tmp;
					}

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New Iterator retreated by n positions
					 */
					inline Iterator 							operator-(difference_type n) const noexcept {
						Iterator tmp = *this;
						tmp -= n;
						return tmp;
					}

					/**
					 * @brief Difference operator
					 * @param other Another Iterator to compare with
					 * @return Difference in positions between the two Iterators
					 */
					inline difference_type 						operator-(const Iterator& other) const noexcept {
						return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
					}

					/**
					 * @brief Equality operator
					 * @param other Another Iterator to compare with
					 * @return True if both Iterators point to the same position in the same Rows
					 */
					inline bool									operator==(const Iterator& other) const noexcept {
						return m_rows == other.m_rows && m_index == other.m_index;
					}

					/**
					 * @brief Inequality operator
					 * @param other Another Iterator to compare with
					 * @return True if both Iterators point to different positions or different Rowss
					 */
					inline bool									operator!=(const Iterator& other) const noexcept {
						return !(*this == other);
					}

				private:
					friend class Rows;

					/**
					 * @brief Constructor
					 * @param rows Pointer to the Rows
					 * @param index Index within the Rows
					 */
					Iterator(Rows* rows, std::size_t index) noexcept:
					m_rows(rows), m_index(index) {}

					Rows* m_rows;								///< Pointer to the Rows
					std::size_t m_index;						///< Current index within the Rows
			};

			class STORMBYTE_DATABASE_PUBLIC ConstIterator {
				public:
					using iterator_category						= std::random_access_iterator_tag;
					using value_type							= const Row;
					using difference_type						= std::ptrdiff_t;
					using pointer								= const Row*;
					using reference								= const Row&;

					/**
					 * @brief Dereference operator
					 * @return Reference to the current Value
					 */
					inline reference							operator*() const {
						return (*m_rows)[m_index];
					}

					/**
					 * @brief ArRows operator
					 * @return Pointer to the current Value
					 */
					inline pointer								operator->() const {
						return &(*m_rows)[m_index];
					}

					/**
					 * @brief Pre-increment operator
					 * @return Reference to the incremented ConstIterator
					 */
					inline ConstIterator&						operator++() {
						++m_index;
						return *this;
					}

					/**
					 * @brief Post-increment operator
					 * @return ConstIterator before increment
					 */
					inline ConstIterator						operator++(int) {
						ConstIterator tmp = *this;
						++(*this);
						return tmp;
					}

					/**
					 * @brief Pre-decrement operator
					 * @return Reference to the decremented ConstIterator
					 */
					inline ConstIterator& 						operator--() noexcept {
						--m_index;
						return *this;
					}

					/**
					 * @brief Post-decrement operator
					 * @return ConstIterator before decrement
					 */
					inline ConstIterator 						operator--(int) noexcept {
						ConstIterator tmp = *this;
						--(*this);
						return tmp;
					}

					/**
					 * @brief Addition assignment operator
					 * @param n Number of positions to advance
					 * @return Reference to the advanced ConstIterator
					 */
					inline ConstIterator& 						operator+=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) + n);
						return *this;
					}

					/**
					 * @brief Subtraction assignment operator
					 * @param n Number of positions to retreat
					 * @return Reference to the retreated ConstIterator
					 */
					inline ConstIterator& 						operator-=(difference_type n) noexcept {
						m_index = static_cast<std::size_t>(static_cast<difference_type>(m_index) - n);
						return *this;
					}

					/**
					 * @brief Addition operator
					 * @param n Number of positions to advance
					 * @return New ConstIterator advanced by n positions
					 */
					inline ConstIterator 						operator+(difference_type n) const noexcept {
						ConstIterator tmp = *this;
						tmp += n;
						return tmp;
					}

					/**
					 * @brief Subtraction operator
					 * @param n Number of positions to retreat
					 * @return New ConstIterator retreated by n positions
					 */
					inline ConstIterator 						operator-(difference_type n) const noexcept {
						ConstIterator tmp = *this;
						tmp -= n;
						return tmp;
					}

					/**
					 * @brief Difference operator
					 * @param other Another ConstIterator to compare with
					 * @return Difference in positions between the two ConstIterators
					 */
					inline difference_type 						operator-(const ConstIterator& other) const noexcept {
						return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
					}

					/**
					 * Random access by offset from const iterator
					 */
					inline reference operator[](difference_type n) const {
						return (*m_rows)[m_index + static_cast<std::size_t>(n)];
					}

					/**
					 * @brief Equality operator
					 * @param other Another ConstIterator to compare with
					 * @return True if both ConstIterators point to the same position in the same Rows
					 */
					inline bool									operator==(const ConstIterator& other) const noexcept {
						return m_rows == other.m_rows && m_index == other.m_index;
					}

					/**
					 * @brief Inequality operator
					 * @param other Another ConstIterator to compare with
					 * @return True if both ConstIterators do not point to the same position in the same Rows
					 */
					inline bool									operator!=(const ConstIterator& other) const noexcept {
						return !(*this == other);
					}

				private:
					friend class Rows;

					/**
					 * @brief Constructor
					 * @param Rows Pointer to the Rows
					 * @param index Index within the Rows
					 */
					ConstIterator(const Rows* rows, std::size_t index) noexcept:
					m_rows(rows), m_index(index) {}

					const Rows* m_rows;							///< Pointer to the Rows
					std::size_t m_index;						///< Current index within the Rows
			};

			using iterator = Iterator;
			using const_iterator = ConstIterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			/**
			 * @brief Adds a row to the Rows
			 * @param row Row to add
			 */
			Rows() noexcept										= default;

			/**
			 * @brief Copy Constructor
			 * @param other Other Rows to copy from
			 */
			Rows(const Rows& other)								= default;

			/**
			 * @brief Move Constructor
			 * @param other Other Rows to move from
			 */
			Rows(Rows&& other) noexcept							= default;

			/**
			 * @brief Destructor
			 */
			~Rows() noexcept									= default;

			/**
			 * @brief Copy Assignment Operator
			 * @param other Other Rows to copy from
			 * @return Reference to this Rows
			 */
			Rows& operator=(const Rows& other)					= default;

			/**
			 * @brief Move Assignment Operator
			 * @param other Other Rows to move from
			 * @return Reference to this Rows
			 */
			Rows& operator=(Rows&& other) noexcept				= default;

			/**
			 * @brief Gets the row at the specified index
			 * @param index Index of the row to retrieve
			 * @return Reference to the Row at the specified index
			 * @throw OutOfBounds if index is out of bounds
			 */
			const Row&											operator[](std::size_t index) const &;

			/**
			 * @brief Gets the row at the specified index (lvalue)
			 * @param index Index of the row to retrieve
			 * @return Reference to the Row at the specified index
			 * @throw OutOfBounds if index is out of bounds
			 */
			Row&												operator[](std::size_t index) &;

			/**
			 * @brief Gets the row at the specified index (rvalue)
			 * @param index Index of the row to retrieve
			 * @return Row at the specified index (moved)
			 * @throw OutOfBounds if index is out of bounds
			 */
			Row													operator[](std::size_t index) &&;

			/**
			 * @brief Adds a row to the Rows
			 * @param row Row to add
			 */
			inline void 										Add(Row&& row) {
				m_rows.emplace_back(std::move(row));
			}

			/**
			 * @brief Gets begin iterator
			 * @return Iterator to the first element
			 */
			inline iterator 									begin() noexcept {
				return Iterator(this, 0);
			}

			/**
			 * @brief Gets end iterator
			 * @return Iterator to past the last element
			 */
			inline iterator 									end() noexcept {
				return Iterator(this, m_rows.size());
			}

			/**
			 * @brief Gets const begin iterator
			 * @return ConstIterator to the first element
			 */
			inline const_iterator								begin() const noexcept {
				return ConstIterator(this, 0);
			}

			/**
			 * @brief Gets const end iterator
			 * @return ConstIterator to past the last element
			 */
			inline const_iterator								end() const noexcept {
				return ConstIterator(this, m_rows.size());
			}

			/**
			 * @brief Gets const begin iterator
			 * @return ConstIterator to the first element
			 */
			inline const_iterator								cbegin() const noexcept {
				return ConstIterator(this, 0);
			}

			/**
			 * @brief Gets const end iterator
			 * @return ConstIterator to past the last element
			 */
			inline const_iterator								cend() const noexcept {
				return ConstIterator(this, m_rows.size());
			}

			/**
			 * @brief Gets reverse begin iterator
			 * @return Reverse iterator to the last element
			 */
			inline reverse_iterator 							rbegin() noexcept {
				return reverse_iterator(end());
			}

			/**
			 * @brief Gets reverse end iterator
			 * @return Reverse iterator to before the first element
			 */
			inline reverse_iterator 							rend() noexcept {
				return reverse_iterator(begin());
			}

			/**
			 * @brief Gets const reverse begin iterator
			 * @return Const reverse iterator to the last element
			 */
			inline const_reverse_iterator 						rbegin() const noexcept {
				return const_reverse_iterator(end());
			}

			/**
			 * @brief Gets const reverse end iterator
			 * @return Const reverse iterator to before the first element
			 */
			inline const_reverse_iterator 						rend() const noexcept {
				return const_reverse_iterator(begin());
			}

			/**
			 * @brief Gets the number of rows
			 * @return Number of rows
			 */
			inline std::size_t									Count() const noexcept {
				return m_rows.size();
			}

		private:
			std::vector<Row> m_rows;							///< Internal storage of values
	};
}
