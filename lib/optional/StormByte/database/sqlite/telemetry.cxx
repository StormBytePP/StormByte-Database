/*
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */
#include <StormByte/database/sqlite/telemetry.hxx>
#include <sqlite3.h>

using namespace StormByte::Database::SQLite;

Telemetry::Telemetry() noexcept = default;
Telemetry::~Telemetry() noexcept = default;

std::uint64_t Telemetry::BusyErrors() const noexcept { return m_busy_errors.load(std::memory_order_acquire); }
std::uint64_t Telemetry::ConstraintErrors() const noexcept { return m_constraint_errors.load(std::memory_order_acquire); }
std::uint64_t Telemetry::IoErrors() const noexcept { return m_io_errors.load(std::memory_order_acquire); }
std::uint64_t Telemetry::CorruptionErrors() const noexcept { return m_corruption_errors.load(std::memory_order_acquire); }

void Telemetry::RecordEvent(const BackendEvent event) noexcept {
	StormByte::Database::Telemetry::RecordEvent(event);
	switch (event) {
		case BackendEvent::Busy: m_busy_errors.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::Constraint: m_constraint_errors.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::Io: m_io_errors.fetch_add(1, std::memory_order_relaxed); break;
		default: break;
	}
}

void Telemetry::RecordSQLiteResult(const int result_code) noexcept {
	switch (result_code & 0xff) {
		case SQLITE_BUSY:
		case SQLITE_LOCKED: RecordEvent(BackendEvent::Busy); break;
		case SQLITE_CONSTRAINT: RecordEvent(BackendEvent::Constraint); break;
		case SQLITE_IOERR: RecordEvent(BackendEvent::Io); break;
		case SQLITE_CORRUPT:
		case SQLITE_NOTADB:
			m_corruption_errors.fetch_add(1, std::memory_order_relaxed);
			RecordEvent(BackendEvent::Other);
			break;
		default: RecordEvent(BackendEvent::Other); break;
	}
}

Telemetry::operator StormByte::Safe::String() const {
	std::string text{static_cast<std::string_view>(StormByte::Database::Telemetry::operator StormByte::Safe::String())};
	text += " SQLite{busy=" + std::to_string(BusyErrors());
	text += ",constraints=" + std::to_string(ConstraintErrors());
	text += ",io=" + std::to_string(IoErrors());
	text += ",corruption=" + std::to_string(CorruptionErrors()) + "}";
	return StormByte::Safe::String(std::string_view{text});
}
