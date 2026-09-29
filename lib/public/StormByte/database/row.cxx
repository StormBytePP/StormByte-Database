/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * StormByte-Database original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte-Database source in this
 * repository. They do not cover other StormByte modules or any third-party
 * material shipped with this repository (including everything under
 * thirdparty/, and in particular the bundled StormByte-Logger tree and
 * the PostgreSQL, MariaDB and SQLite trees), which remain under their own
 * licenses.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte-Database is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte-Database. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/database/exception.hxx>
#include <StormByte/database/row.hxx>

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace StormByte::Database;

struct Row::Columns {
	std::vector<NamedValue> values;
	mutable std::optional<std::unordered_map<std::string, StormByte::Size>> name_index;
};

Row::Row() noexcept = default;

Row::Row(const Row& other)
	: m_columns(other.m_columns ? std::make_unique<Columns>(*other.m_columns) : nullptr) {}

Row::Row(Row&& other) noexcept = default;

Row::~Row() noexcept = default;
Row& Row::operator=(const Row& other) {
	if (this != &other)
		m_columns = other.m_columns ? std::make_unique<Columns>(*other.m_columns) : nullptr;

	return *this;
}

Row& Row::operator=(Row&& other) noexcept = default;

bool Row::operator==(const Row& other) const {
	if (!m_columns || !other.m_columns)
		return empty() && other.empty();
	return m_columns->values == other.m_columns->values;
}

bool Row::operator!=(const Row& other) const {
	return !(*this == other);
}

Row::iterator Row::begin() noexcept {
	return m_columns && !m_columns->values.empty() ? m_columns->values.data() : nullptr;
}

Row::const_iterator Row::begin() const noexcept {
	return m_columns && !m_columns->values.empty() ? m_columns->values.data() : nullptr;
}

Row::iterator Row::end() noexcept {
	return m_columns && !m_columns->values.empty() ? m_columns->values.data() + m_columns->values.size() : nullptr;
}

Row::const_iterator Row::end() const noexcept {
	return m_columns && !m_columns->values.empty() ? m_columns->values.data() + m_columns->values.size() : nullptr;
}

Row::const_iterator Row::cbegin() const noexcept {
	return begin();
}

Row::const_iterator Row::cend() const noexcept {
	return end();
}

Row::reverse_iterator Row::rbegin() noexcept {
	return reverse_iterator(end());
}

Row::reverse_iterator Row::rend() noexcept {
	return reverse_iterator(begin());
}

Row::const_reverse_iterator Row::rbegin() const noexcept {
	return const_reverse_iterator(end());
}

Row::const_reverse_iterator Row::rend() const noexcept {
	return const_reverse_iterator(begin());
}

Row::const_reverse_iterator Row::crbegin() const noexcept {
	return rbegin();
}

Row::const_reverse_iterator Row::crend() const noexcept {
	return rend();
}

StormByte::Size Row::size() const noexcept {
	return StormByte::Size{m_columns ? m_columns->values.size() : 0};
}

bool Row::empty() const noexcept {
	return !m_columns || m_columns->values.empty();
}

void Row::add(std::string_view columnName, Value&& value) {
	if (!m_columns)
		m_columns = std::make_unique<Columns>();
	m_columns->values.emplace_back(columnName, std::move(value));
	m_columns->name_index.reset();
}

void Row::add(NamedValue value) {
	if (!m_columns)
		m_columns = std::make_unique<Columns>();
	m_columns->values.emplace_back(std::move(value));
	m_columns->name_index.reset();
}

bool Row::has_item(const NamedValue& value) const {
	return m_columns && std::find(m_columns->values.begin(), m_columns->values.end(), value) != m_columns->values.end();
}

StormByte::Size Row::Count() const noexcept {
	return StormByte::Size{size()};
}

void Row::BuildNameIndex() const {
	if (!m_columns || m_columns->name_index)
		return;
	std::unordered_map<std::string, StormByte::Size> index;
	index.reserve(m_columns->values.size());
	StormByte::Size i{};
	for (const auto& value : m_columns->values) {
		index.emplace(value.Name(), i);
		++i;
	}

	m_columns->name_index = std::move(index);
}

const Value& Row::operator[](std::string_view columnName) const & {
	if (!m_columns)
		throw ColumnNotFound(columnName);
	BuildNameIndex();
	auto it = m_columns->name_index->find(std::string{columnName});
	if (it == m_columns->name_index->end())
		throw ColumnNotFound(columnName);
	return m_columns->values[static_cast<std::size_t>(it->second)];
}

Value& Row::operator[](std::string_view columnName) & {
	return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnName));
}

Value Row::operator[](std::string_view columnName) && {
	if (!m_columns)
		throw ColumnNotFound(columnName);
	BuildNameIndex();
	auto it = m_columns->name_index->find(std::string{columnName});
	if (it == m_columns->name_index->end())
		throw ColumnNotFound(columnName);
	return std::move(m_columns->values[static_cast<std::size_t>(it->second)]);
}

const NamedValue& Row::operator[](StormByte::Size index) const & {
	if (!m_columns || index >= size())
		throw OutOfBounds(index, size());
	return m_columns->values[static_cast<std::size_t>(index)];
}

NamedValue& Row::operator[](StormByte::Size index) & {
	return const_cast<NamedValue&>(static_cast<const Row*>(this)->operator[](index));
}

NamedValue Row::operator[](StormByte::Size index) && {
	if (!m_columns || index >= size())
		throw OutOfBounds(index, size());
	return std::move(m_columns->values[static_cast<std::size_t>(index)]);
}
