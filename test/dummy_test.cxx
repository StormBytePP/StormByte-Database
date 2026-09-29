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
#include <StormByte/database/value.hxx>
#include <StormByte/test_handlers.h>

#include <iostream>
#include <limits>
#include <string>

using namespace StormByte::Database;

int test_component_prefixed_exceptions() {
	int result = 0;
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database: generic error"), std::string(Exception("generic error").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.Connection: connection failed"), std::string(ConnectionError("connection failed").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.WrongValueType.Value: expected integer"), std::string(WrongValueType("Value", "expected {}", "integer").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.ColumnNotFound: Column 'id' not found"), std::string(ColumnNotFound("id").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.OutOfBounds: Position 3 is out of bounds for size 2"), std::string(OutOfBounds(3, 2).what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.Query.PreparedSTMT: Statement 'users' not found"), std::string(UnknownSTMT("users").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.Query.Execute: Error executing query: syntax error"), std::string(ExecuteError("syntax error").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte.Database.Transaction: Unable to begin transaction: disconnected"), std::string(TransactionError("disconnected").what()));
	RETURN_TEST("test_component_prefixed_exceptions", result);
}

int test_invalid_value_conversions_throw() {
	int result = 0;
	bool threw = false;
	try {
		(void)Value().Get<int>();
	} catch (const WrongValueType&) {
		threw = true;
	}

	ASSERT_TRUE("test_invalid_value_conversions_throw", threw);

	threw = false;
	try {
		(void)Value("text").Get<int>();
	} catch (const WrongValueType&) {
		threw = true;
	}

	ASSERT_TRUE("test_invalid_value_conversions_throw", threw);

	threw = false;
	try {
		(void)Value(-1).Get<unsigned int>();
	} catch (const WrongValueType&) {
		threw = true;
	}

	ASSERT_TRUE("test_invalid_value_conversions_throw", threw);

	threw = false;
	try {
		(void)Value(std::numeric_limits<unsigned long int>::max()).Get<int>();
	} catch (const WrongValueType&) {
		threw = true;
	}

	ASSERT_TRUE("test_invalid_value_conversions_throw", threw);

	threw = false;
	try {
		(void)Value(1.5).Get<int>();
	} catch (const WrongValueType&) {
		threw = true;
	}

	ASSERT_TRUE("test_invalid_value_conversions_throw", threw);
	RETURN_TEST("test_invalid_value_conversions_throw", result);
}

int main() {
	int result = 0;
	result += test_component_prefixed_exceptions();
	result += test_invalid_value_conversions_throw();
	if (result == 0) {
		std::cout << "All tests passed successfully.\n";
	} else {
		std::cout << result << " tests failed.\n";
	}

	return result;
}
