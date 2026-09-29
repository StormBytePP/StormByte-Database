#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/test_handlers.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>

/**
 * @file backend_contract.hxx
 * @brief Shared behavioral tests for all built-in SQL backends.
 */

/**
 * @brief Verify round-trips for all database value categories and boundary values.
 * @tparam DatabaseType Built-in backend test fixture.
 * @param db Connected database fixture.
 * @param test_name Name used in assertion output.
 * @return Test-handler result.
 */
template <typename DatabaseType>
int verify_scalar_backend_contract(DatabaseType& db, const std::string& test_name) {
	const int signed_int = std::numeric_limits<int>::min();
	const unsigned int unsigned_int = std::numeric_limits<unsigned int>::max();
	const long int signed_long = std::numeric_limits<long int>::min();
	const unsigned long int unsigned_long = static_cast<unsigned long int>(std::numeric_limits<long int>::max());
	const double floating = -12345.625;
	const std::string text = "quoted ' text with \\\\ backslashes";
	const std::array<std::byte, 5> bytes{std::byte{0}, std::byte{0xFF}, std::byte{0}, std::byte{0x7F}, std::byte{0x80}};
	const StormByte::BinaryData blob{bytes.data(), StormByte::ByteSize{bytes.size()}};

	ASSERT_TRUE(test_name, db.Connect());
	ASSERT_TRUE(test_name, db.ExecuteSTMT("insert_scalar_types", signed_int, unsigned_int, signed_long, unsigned_long, floating, text, blob, true, nullptr).has_value());
	ASSERT_TRUE(test_name, db.ExecuteSTMT("insert_scalar_types", signed_int, unsigned_int, signed_long, unsigned_long, floating, text, blob, false, "not null").has_value());

	auto result = db.Query("SELECT signed_integer, unsigned_integer, signed_long, unsigned_long, real_number, text_value, blob_value, flag, nullable_value FROM scalar_types ORDER BY id;");
	ASSERT_TRUE(test_name, result.has_value());
	ASSERT_EQUAL(test_name, 2, result.value().Count());

	for (StormByte::Size row_index{}; row_index < result.value().Count(); ++row_index) {
		const auto& row = result.value()[row_index];
		ASSERT_EQUAL(test_name, signed_int, row[0].template Get<int>());
		ASSERT_EQUAL(test_name, unsigned_int, row[1].template Get<unsigned int>());
		ASSERT_EQUAL(test_name, signed_long, row[2].template Get<long int>());
		ASSERT_EQUAL(test_name, unsigned_long, row[3].template Get<unsigned long int>());
		ASSERT_EQUAL(test_name, floating, row[4].template Get<double>());
		ASSERT_EQUAL(test_name, StormByte::String::String{text}, row[5].template Get<StormByte::String::String>());
		const auto& returned_blob = row[6].template Get<StormByte::BinaryData>();
		ASSERT_EQUAL(test_name, bytes.size(), returned_blob.size());
		for (std::size_t byte_index{}; byte_index < bytes.size(); ++byte_index)
			ASSERT_EQUAL(test_name, bytes[byte_index], returned_blob[byte_index]);
		ASSERT_EQUAL(test_name, row_index == 0, row[7].template Get<bool>());
	}

	ASSERT_TRUE(test_name, result.value()[0][8].IsNull());
	ASSERT_EQUAL(test_name, StormByte::String::String{"not null"}, result.value()[1][8].template Get<StormByte::String::String>());
	auto empty_text = db.Query("SELECT '' AS empty_text;");
	ASSERT_TRUE(test_name, empty_text.has_value());
	ASSERT_EQUAL(test_name, 1, empty_text.value().Count());
	ASSERT_TRUE(test_name, empty_text.value()[0][0].template Get<StormByte::String::String>().empty());
	auto no_rows = db.Query("SELECT 1 WHERE 1 = 0;");
	ASSERT_TRUE(test_name, no_rows.has_value());
	ASSERT_TRUE(test_name, no_rows.value().empty());
	RETURN_TEST(test_name, 0);
}

/**
 * @brief Verify zero-length BLOB, large BLOB and SQL NULL remain distinct.
 * @tparam DatabaseType Built-in backend test fixture.
 * @param db Connected database fixture.
 * @param test_name Name used in assertion output.
 * @return Test-handler result.
 */
template <typename DatabaseType>
int verify_binary_backend_contract(DatabaseType& db, const std::string& test_name) {
	ASSERT_TRUE(test_name, db.Connect());
	ASSERT_TRUE(test_name, db.ExecuteSTMT("insert_blob", StormByte::BinaryData{}).has_value());
	auto empty_blob_rows = db.ExecuteSTMT("select_blob");
	ASSERT_TRUE(test_name, empty_blob_rows.has_value());
	ASSERT_EQUAL(test_name, 1, empty_blob_rows.value().Count());
	ASSERT_FALSE(test_name, empty_blob_rows.value()[0][0].IsNull());
	ASSERT_TRUE(test_name, empty_blob_rows.value()[0][0].template Get<StormByte::BinaryData>().empty());

	StormByte::BinaryData large_blob{StormByte::ByteSize{48 * 1024}};
	for (std::size_t index{}; index < large_blob.size(); ++index)
		large_blob.begin()[index] = static_cast<std::byte>(index % 251);
	ASSERT_TRUE(test_name, db.ExecuteSTMT("insert_blob", large_blob).has_value());
	auto large_blob_rows = db.ExecuteSTMT("select_blob");
	ASSERT_TRUE(test_name, large_blob_rows.has_value());
	const auto& returned_blob = large_blob_rows.value()[0][0].template Get<StormByte::BinaryData>();
	ASSERT_EQUAL(test_name, large_blob.size(), returned_blob.size());
	ASSERT_TRUE(test_name, std::equal(large_blob.begin(), large_blob.end(), returned_blob.begin()));

	ASSERT_TRUE(test_name, db.ExecuteSTMT("insert_null", nullptr).has_value());
	auto null_rows = db.Query("SELECT value FROM nulls ORDER BY id DESC LIMIT 1;");
	ASSERT_TRUE(test_name, null_rows.has_value());
	ASSERT_EQUAL(test_name, 1, null_rows.value().Count());
	ASSERT_TRUE(test_name, null_rows.value()[0][0].IsNull());
	RETURN_TEST(test_name, 0);
}
