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

#include <StormByte/database/named_value.hxx>
#include <StormByte/iterable.hxx>

#include <optional>
#include <unordered_map>
#include <vector>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class Row
	 * @brief One result row: ordered NamedValues with lookup by column name.
	 */
	class STORMBYTE_DATABASE_PUBLIC Row: public Iterable<std::vector<NamedValue>> {
		public:
			/**
			 * @brief Default constructor.
			 */
			Row() noexcept = default;

			/**
			 * @brief Copy constructor.
			 * @param other Source row.
			 */
			Row(const Row& other);

			/**
			 * @brief Move constructor.
			 */
			Row(Row&& other) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~Row() noexcept override = default;

			/**
			 * @brief Copy assignment.
			 * @param other Source row.
			 * @return *this.
			 */
			Row& operator=(const Row& other);

			/**
			 * @brief Move assignment.
			 */
			Row& operator=(Row&& other) noexcept = default;

			/**
			 * @brief Access by column name (const lvalue).
			 * @param columnName Column name.
			 * @return Reference to the value.
			 * @throws ColumnNotFound if the name is absent.
			 */
			const Value& operator[](const std::string& columnName) const &;

			/**
			 * @brief Access by column name (lvalue).
			 * @param columnName Column name.
			 * @return Reference to the value.
			 * @throws ColumnNotFound if the name is absent.
			 */
			Value& operator[](const std::string& columnName) &;

			/**
			 * @brief Access by column name (rvalue).
			 * @param columnName Column name.
			 * @return Value (moved).
			 * @throws ColumnNotFound if the name is absent.
			 */
			Value operator[](const std::string& columnName) &&;

			using Iterable::operator[];

			/**
			 * @brief Append a named column.
			 * @param columnName Column name.
			 * @param value Value to store.
			 */
			inline void add(std::string&& columnName, Value&& value) {
				m_data.emplace_back(std::move(columnName), std::move(value));
				m_name_index.reset();
			}

			/**
			 * @brief Number of columns.
			 * @return Count.
			 */
			inline std::size_t Count() const noexcept {
				return size();
			}

		private:
			mutable std::optional<std::unordered_map<std::string, std::size_t>> m_name_index;	///< Lazy name → index map

			/**
			 * @brief Build m_name_index if missing.
			 */
			void BuildNameIndex() const;
	};
}
