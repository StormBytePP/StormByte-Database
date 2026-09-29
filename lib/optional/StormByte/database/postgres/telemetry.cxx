/*
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */
#include <StormByte/database/postgres/telemetry.hxx>

using namespace StormByte::Database::Postgres;

Telemetry::Telemetry() noexcept = default;
Telemetry::~Telemetry() noexcept = default;

std::uint64_t Telemetry::SerializationConflicts() const noexcept { return m_serialization_conflicts.load(std::memory_order_acquire); }
std::uint64_t Telemetry::Deadlocks() const noexcept { return m_deadlocks.load(std::memory_order_acquire); }
std::uint64_t Telemetry::ConnectionErrors() const noexcept { return m_connection_errors.load(std::memory_order_acquire); }

void Telemetry::RecordEvent(const BackendEvent event) noexcept {
	StormByte::Database::Telemetry::RecordEvent(event);
	switch (event) {
		case BackendEvent::Serialization: m_serialization_conflicts.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::Deadlock: m_deadlocks.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::Connection: m_connection_errors.fetch_add(1, std::memory_order_relaxed); break;
		default: break;
	}
}

void Telemetry::RecordSqlState(const std::string_view sql_state) noexcept {
	if (sql_state == "40001")
		RecordEvent(BackendEvent::Serialization);
	else if (sql_state == "40P01")
		RecordEvent(BackendEvent::Deadlock);
	else if (sql_state.size() >= 2 && sql_state.substr(0, 2) == "23")
		RecordEvent(BackendEvent::Constraint);
	else if (sql_state.size() >= 2 && sql_state.substr(0, 2) == "08")
		RecordEvent(BackendEvent::Connection);
	else if (sql_state == "55P03")
		RecordEvent(BackendEvent::Busy);
	else
		RecordEvent(BackendEvent::Other);
}

Telemetry::operator StormByte::String::String() const {
	std::string text{static_cast<std::string_view>(StormByte::Database::Telemetry::operator StormByte::String::String())};
	text += " PostgreSQL{serialization=" + std::to_string(SerializationConflicts());
	text += ",deadlocks=" + std::to_string(Deadlocks());
	text += ",connection_errors=" + std::to_string(ConnectionErrors()) + "}";
	return StormByte::String::String(std::string_view{text});
}
