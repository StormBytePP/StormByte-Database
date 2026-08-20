/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see <https://www.gnu.org/licenses/>.
 */

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
