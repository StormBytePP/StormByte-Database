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

#include <StormByte/database/exception.hxx>
#include <StormByte/database/row.hxx>
using namespace StormByte::Database;
Row::Row(const Row& other)
	: Iterable(other), m_name_index(other.m_name_index) {}
Row& Row::operator=(const Row& other) {
	if (this != &other) {
		Iterable::operator=(other);
		m_name_index = other.m_name_index;
	}
	return *this;
}
void Row::BuildNameIndex() const {
	if (m_name_index) return;
	std::unordered_map<std::string, std::size_t> index;
	index.reserve(m_data.size());
	for (std::size_t i = 0; i < m_data.size(); ++i) {
		index.emplace(m_data[i].Name(), i);
	}
	m_name_index = std::move(index);
}
const Value& Row::operator[](const std::string& columnName) const & {
	BuildNameIndex();
	auto it = m_name_index->find(columnName);
	if (it == m_name_index->end())
		throw ColumnNotFound(columnName);
	return m_data[it->second];
}
Value& Row::operator[](const std::string& columnName) & {
	return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnName));
}
Value Row::operator[](const std::string& columnName) && {
	BuildNameIndex();
	auto it = m_name_index->find(columnName);
	if (it == m_name_index->end())
		throw ColumnNotFound(columnName);
	return std::move(m_data[it->second]);
}
