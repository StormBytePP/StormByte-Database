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
	class STORMBYTE_DATABASE_PUBLIC Rows: public Iterable<std::vector<Row>> {
		public:
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
			~Rows() noexcept override							= default;

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
			 * @brief Gets the number of rows
			 * @return Number of rows
			 */
			inline std::size_t									Count() const noexcept {
				return size();
			}
	};
}
