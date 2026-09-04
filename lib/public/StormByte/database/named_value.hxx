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

#include <StormByte/database/value.hxx>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class NamedValue
	 * @brief Value plus column name.
	 */
	class STORMBYTE_DATABASE_PUBLIC NamedValue: public Value {
		public:
			/**
			 * @brief Copy name and value.
			 * @param name Column name.
			 * @param value Value.
			 */
			NamedValue(const std::string& name, const Value& value) noexcept:
			Value(value), m_name(name) {}

			/**
			 * @brief Move name and value.
			 * @param name Column name.
			 * @param value Value.
			 */
			NamedValue(std::string&& name, Value&& value) noexcept:
			Value(std::move(value)), m_name(std::move(name)) {}

			/**
			 * @brief Copy constructor.
			 */
			NamedValue(const NamedValue& other) = default;

			/**
			 * @brief Move constructor.
			 */
			NamedValue(NamedValue&& other) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~NamedValue() noexcept override = default;

			/**
			 * @brief Copy assignment.
			 */
			NamedValue& operator=(const NamedValue& other) = default;

			/**
			 * @brief Move assignment.
			 */
			NamedValue& operator=(NamedValue&& other) noexcept = default;

			/**
			 * @brief Equality (name and value).
			 * @param other Other value.
			 * @return true if equal.
			 */
			inline bool operator==(const NamedValue& other) const noexcept {
				return m_name == other.m_name && Value::operator==(other);
			}

			/**
			 * @brief Inequality.
			 * @param other Other value.
			 * @return true if not equal.
			 */
			inline bool operator!=(const NamedValue& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @brief Column name.
			 * @return Name.
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

		private:
			std::string m_name;	///< Column name
	};
}
