#pragma once

#include <StormByte/database/row.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Rows
	 * @brief Ordered collection of result rows.
	 */
	class STORMBYTE_DATABASE_PUBLIC Rows: public Iterable<std::vector<Row>> {
		public:
			/**
			 * Default constructor.
			 */
			Rows() noexcept = default;

			/**
			 * Copy constructor.
			 */
			Rows(const Rows& other) = default;

			/**
			 * Move constructor.
			 */
			Rows(Rows&& other) noexcept = default;

			/**
			 * Destructor.
			 */
			~Rows() noexcept override = default;

			/**
			 * Copy assignment.
			 */
			Rows& operator=(const Rows& other) = default;

			/**
			 * Move assignment.
			 */
			Rows& operator=(Rows&& other) noexcept = default;

			/**
			 * @return Number of rows.
			 */
			inline std::size_t Count() const noexcept {
				return size();
			}
	};
}
