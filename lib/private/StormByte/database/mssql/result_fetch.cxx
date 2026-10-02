/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte-Database.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/database/mssql/result_fetch.hxx>
#include <StormByte/database/rows.hxx>
#include <StormByte/database/value.hxx>

#include <sybdb.h>

#include <algorithm>
#include <limits>
#include <string>
#include <string_view>
#include <charconv>
#include <utility>
#include <vector>

using namespace StormByte::Database;

namespace {
	using StormByte::Database::Value;

	StormByte::Expected<Value, ExecuteError> ReadValue(DBPROCESS* process, const int column,
			const bool is_null) noexcept {
		if (is_null)
			return Value{};
		BYTE* data = dbdata(process, column);
		const DBINT length = dbdatlen(process, column);
		if (length < 0)
			return StormByte::Unexpected<ExecuteError>("DB-Library returned a negative column length");

		const int type = dbcoltype(process, column);
		if (!data && length == 0) {
			switch (type) {
				case SYBBINARY:
				case SYBVARBINARY:
				case SYBIMAGE:
					return Value{StormByte::BinaryData{}};
				default:
					return Value{std::string_view{}};
			}
		}
		if (!data)
			return StormByte::Unexpected<ExecuteError>("DB-Library returned no data for a non-NULL MSSQL value");
		switch (type) {
			case SYBINT1:
			case SYBINT2:
			case SYBINT4:
			case SYBINT8:
			case SYBINTN: {
				DBBIGINT value{};
				if (dbconvert(process, type, data, length, SYBINT8,
						reinterpret_cast<BYTE*>(&value), static_cast<DBINT>(sizeof(value))) == FAIL)
					return StormByte::Unexpected<ExecuteError>("Could not convert MSSQL integer result");
				if (value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max())
					return Value{static_cast<int>(value)};
				return Value{static_cast<long long int>(value)};
			}
			case SYBBIT:
			case SYBBITN: {
				DBBIT value{};
				if (dbconvert(process, type, data, length, SYBBIT,
						reinterpret_cast<BYTE*>(&value), static_cast<DBINT>(sizeof(value))) == FAIL)
					return StormByte::Unexpected<ExecuteError>("Could not convert MSSQL bit result");
				return Value{value != 0};
			}
			case SYBREAL:
			case SYBFLT8:
			case SYBFLTN: {
				DBFLT8 value{};
				if (dbconvert(process, type, data, length, SYBFLT8,
						reinterpret_cast<BYTE*>(&value), static_cast<DBINT>(sizeof(value))) == FAIL)
					return StormByte::Unexpected<ExecuteError>("Could not convert MSSQL floating-point result");
				return Value{static_cast<double>(value)};
			}
			case SYBBINARY:
			case SYBVARBINARY:
			case SYBIMAGE: {
				StormByte::BinaryData value{reinterpret_cast<const std::byte*>(data), StormByte::ByteSize{static_cast<std::size_t>(length)}};
				return Value{std::move(value)};
			}
			case SYBNUMERIC:
			case SYBDECIMAL:
			case SYBMONEY:
			case SYBMONEY4:
			case SYBMONEYN: {
				std::vector<char> text(128);
				const DBINT converted = dbconvert(process, type, data, length, SYBVARCHAR,
					reinterpret_cast<BYTE*>(text.data()), static_cast<DBINT>(text.size()));
				if (converted == FAIL)
					return StormByte::Unexpected<ExecuteError>("Could not convert MSSQL decimal result");
				const char* begin = text.data();
				const char* end = begin + converted;
				if (std::string_view{begin, static_cast<std::size_t>(converted)}.find_first_of(".eE") == std::string_view::npos) {
					if (begin != end && *begin == '-') {
						long long int value{};
						const auto parsed = std::from_chars(begin, end, value);
						if (parsed.ec == std::errc{} && parsed.ptr == end) {
							if (value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max())
								return Value{static_cast<int>(value)};
							return Value{value};
						}
					} else {
						unsigned long long int value{};
						const auto parsed = std::from_chars(begin, end, value);
						if (parsed.ec == std::errc{} && parsed.ptr == end) {
							if (value <= std::numeric_limits<unsigned int>::max())
								return Value{static_cast<unsigned int>(value)};
							return Value{value};
						}
					}
				}
				double value{};
				const auto parsed = std::from_chars(begin, end, value);
				if (parsed.ec == std::errc{} && parsed.ptr == end)
					return Value{value};
				return StormByte::Unexpected<ExecuteError>("Invalid MSSQL decimal result");
			}
			default: {
				const std::size_t source_length = static_cast<std::size_t>(length);
				if (source_length > (static_cast<std::size_t>(std::numeric_limits<DBINT>::max()) - 256) / 4)
					return StormByte::Unexpected<ExecuteError>("MSSQL text result exceeds supported length");
				std::vector<char> text(std::max<std::size_t>(4096, source_length * 4 + 256));
				const DBINT converted = dbconvert(process, type, data, length, SYBVARCHAR,
					reinterpret_cast<BYTE*>(text.data()), static_cast<DBINT>(text.size()));
				if (converted == FAIL)
					return StormByte::Unexpected<ExecuteError>("Could not convert MSSQL text result");
				const std::size_t text_length = std::min<std::size_t>(static_cast<std::size_t>(converted), text.size());
				return Value{std::string_view{text.data(), text_length}};
			}
		}
	}
}

ExpectedRows StormByte::Database::MSSQL::StepResults(DBPROCESS* process) noexcept {
	if (!process)
		return StormByte::Unexpected<QueryException>(ExecuteError("Invalid DBPROCESS provided"));

	Rows rows;
	for (;;) {
		const RETCODE result_status = dbresults(process);
		if (result_status == NO_MORE_RESULTS)
			break;
		if (result_status == FAIL)
			return StormByte::Unexpected<QueryException>(ExecuteError("DB-Library failed while reading query results"));

		const int column_count = dbnumcols(process);
		if (column_count < 0)
			return StormByte::Unexpected<QueryException>(ExecuteError("DB-Library returned an invalid column count"));
		std::vector<DBINT> null_indicators(static_cast<std::size_t>(column_count));
		for (int column = 1; column <= column_count; ++column) {
			if (dbnullbind(process, column, &null_indicators[static_cast<std::size_t>(column - 1)]) == FAIL)
				return StormByte::Unexpected<QueryException>(ExecuteError("DB-Library could not bind an MSSQL NULL indicator"));
		}

		for (;;) {
			const STATUS row_status = dbnextrow(process);
			if (row_status == NO_MORE_ROWS)
				break;
			if (row_status == FAIL)
				return StormByte::Unexpected<QueryException>(ExecuteError("DB-Library failed while fetching a row"));
			if (row_status != REG_ROW)
				continue;

			Row row;
			for (int column = 1; column <= column_count; ++column) {
				const char* column_name = dbcolname(process, column);
				const std::string_view name{column_name ? column_name : ""};
				auto value = ReadValue(process, column,
					null_indicators[static_cast<std::size_t>(column - 1)] == -1);
				if (!value)
					return StormByte::Unexpected<QueryException>(*value.error());
				row.add(name, std::move(*value));
			}
			rows.add(std::move(row));
		}
	}
	return rows;
}