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

#include <StormByte/database/transaction.hxx>
#include <StormByte/database/database.hxx>
using namespace StormByte::Database;
Transaction::Transaction(Database& db) noexcept
	: m_db(&db), m_active(true) {}
Transaction::Transaction(Transaction&& other) noexcept
	: m_db(other.m_db), m_active(other.m_active) {
	other.m_db = nullptr;
	other.m_active = false;
}
Transaction& Transaction::operator=(Transaction&& other) noexcept {
	if (this != &other) {
		if (m_active && m_db)
			m_db->RollbackTransaction();
		m_db = other.m_db;
		m_active = other.m_active;
		other.m_db = nullptr;
		other.m_active = false;
	}
	return *this;
}
Transaction::~Transaction() noexcept {
	if (m_active && m_db)
		m_db->RollbackTransaction();
}
void Transaction::Commit() {
	if (!m_active || !m_db)
		return;
	m_db->CommitTransaction();
	m_active = false;
}
void Transaction::Rollback() {
	if (!m_active || !m_db)
		return;
	m_db->RollbackTransaction();
	m_active = false;
}
