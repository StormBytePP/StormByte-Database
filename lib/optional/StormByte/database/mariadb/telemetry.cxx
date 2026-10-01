/*
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */
#include <StormByte/database/mariadb/telemetry.hxx>

using namespace StormByte::Database::MariaDB;

Telemetry::Telemetry() noexcept = default;
Telemetry::~Telemetry() noexcept = default;

std::uint64_t Telemetry::Deadlocks() const noexcept { return m_deadlocks.load(std::memory_order_acquire); }
std::uint64_t Telemetry::LockTimeouts() const noexcept { return m_lock_timeouts.load(std::memory_order_acquire); }
std::uint64_t Telemetry::Warnings() const noexcept { return m_warnings.load(std::memory_order_acquire); }

void Telemetry::RecordEvent(const BackendEvent event) noexcept {
	StormByte::Database::Telemetry::RecordEvent(event);
	switch (event) {
		case BackendEvent::Deadlock: m_deadlocks.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::LockTimeout: m_lock_timeouts.fetch_add(1, std::memory_order_relaxed); break;
		case BackendEvent::Warning: m_warnings.fetch_add(1, std::memory_order_relaxed); break;
		default: break;
	}
}

void Telemetry::RecordMariaDBError(const unsigned int error_code) noexcept {
	if (error_code == 1213)
		RecordEvent(BackendEvent::Deadlock);
	else if (error_code == 1205)
		RecordEvent(BackendEvent::LockTimeout);
	else if (error_code == 1062 || error_code == 1451 || error_code == 1452)
		RecordEvent(BackendEvent::Constraint);
	else
		RecordEvent(BackendEvent::Other);
}

void Telemetry::RecordMariaDBWarnings(const std::uint64_t count) noexcept {
	RecordEvents(BackendEvent::Warning, count);
	m_warnings.fetch_add(count, std::memory_order_relaxed);
}

Telemetry::operator StormByte::Safe::String() const {
	std::string text{static_cast<std::string_view>(StormByte::Database::Telemetry::operator StormByte::Safe::String())};
	text += " MariaDB{deadlocks=" + std::to_string(Deadlocks());
	text += ",lock_timeouts=" + std::to_string(LockTimeouts());
	text += ",warnings=" + std::to_string(Warnings()) + "}";
	return StormByte::Safe::String(std::string_view{text});
}
