/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#pragma once

#include <StormByte/database/row.hxx>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class Rows
	 * @brief Ordered collection of result rows.
	 */
	class STORMBYTE_DATABASE_PUBLIC Rows: public Iterable<std::vector<Row>> {
		public:
			/**
			 * @brief Default constructor.
			 */
			Rows() noexcept = default;

			/**
			 * @brief Copy constructor.
			 */
			Rows(const Rows& other) = default;

			/**
			 * @brief Move constructor.
			 */
			Rows(Rows&& other) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~Rows() noexcept override = default;

			/**
			 * @brief Copy assignment.
			 */
			Rows& operator=(const Rows& other) = default;

			/**
			 * @brief Move assignment.
			 */
			Rows& operator=(Rows&& other) noexcept = default;

			/**
			 * @brief Number of rows.
			 * @return Count.
			 */
			inline std::size_t Count() const noexcept {
				return size();
			}
	};
}
