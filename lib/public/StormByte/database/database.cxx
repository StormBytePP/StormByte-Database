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

#include <StormByte/database/database.hxx>
using namespace StormByte::Database;
bool Database::Connect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect enter" << std::endl;
	DoPreConnect();
	bool result = DoConnect();
	if (result) {
		m_connected = true;
		DoPostConnect();
	}
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect leave (" << (result ? "ok" : "fail") << ")" << std::endl;
	return result;
}
void Database::Disconnect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect enter" << std::endl;
	DoPreDisconnect();
	DoDisconnect();
	DoPostDisconnect();
	m_connected = false;
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect leave" << std::endl;
}
void Database::PrepareSTMT(std::string&& name, std::string&& query) noexcept {
	DoPrepareSTMT(std::move(name), std::move(query));
}
void Database::DoPrepareSTMT(std::string&& name, std::string&& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Preparing statement '" << name << "': " << query << std::endl;
	std::unique_ptr<PreparedSTMT> prepared = CreatePreparedSTMT(std::move(name), std::move(query));
	if (prepared)
		m_prepared_stmts.emplace(prepared->Name(), std::move(prepared));
}
Transaction Database::BeginTransaction(IsolationLevel level) {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "BeginTransaction" << std::endl;
	DoBeginTransaction(level);
	return Transaction(*this);
}
void Database::CommitTransaction() {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "CommitTransaction" << std::endl;
	DoSilentQuery("COMMIT;");
}
void Database::RollbackTransaction() {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "RollbackTransaction" << std::endl;
	DoSilentQuery("ROLLBACK;");
}
