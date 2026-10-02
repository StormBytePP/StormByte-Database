/*
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/database/mssql/telemetry.hxx>

#include <string>
#include <string_view>

using namespace StormByte::Database::MSSQL;

Telemetry::Telemetry() noexcept = default;
Telemetry::~Telemetry() noexcept = default;

std::uint64_t Telemetry::Errors() const noexcept {
	return m_errors.load(std::memory_order_acquire);
}

void Telemetry::RecordError() noexcept {
	m_errors.fetch_add(1, std::memory_order_relaxed);
	RecordEvent(BackendEvent::Other);
}

void Telemetry::RecordEvent(const BackendEvent event) noexcept {
	StormByte::Database::Telemetry::RecordEvent(event);
	if (event == BackendEvent::Connection)
		m_errors.fetch_add(1, std::memory_order_relaxed);
}

Telemetry::operator StormByte::Safe::String() const {
	std::string text{static_cast<std::string_view>(StormByte::Database::Telemetry::operator StormByte::Safe::String())};
	text += " MSSQL{errors=" + std::to_string(Errors()) + "}";
	return StormByte::Safe::String(std::string_view{text});
}