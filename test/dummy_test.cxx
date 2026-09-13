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
#include <StormByte/test_handlers.h>

#include <iostream>
#include <string>

using namespace StormByte::Database;

int test_component_prefixed_exceptions() {
	int result = 0;
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database: generic error"), std::string(Exception("generic error").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::Connection: connection failed"), std::string(ConnectionError("connection failed").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::WrongValueType::Value: expected integer"), std::string(WrongValueType("Value", "expected {}", "integer").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::ColumnNotFound: Column 'id' not found"), std::string(ColumnNotFound("id").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::OutOfBounds: Position 3 is out of bounds for size 2"), std::string(OutOfBounds(3, 2).what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::Query::PreparedSTMT: Statement 'users' not found"), std::string(UnknownSTMT("users").what()));
	ASSERT_EQUAL("test_component_prefixed_exceptions", std::string("StormByte::Database::Query::Execute: Error executing query: syntax error"), std::string(ExecuteError("syntax error").what()));
	RETURN_TEST("test_component_prefixed_exceptions", result);
}

int main() {
	int result = test_component_prefixed_exceptions();
	if (result == 0) {
		std::cout << "All tests passed successfully.\n";
	} else {
		std::cout << result << " tests failed.\n";
	}
	return result;
}
