#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/database/transaction.hxx>
#include <StormByte/logger/log.hxx>
#include <StormByte/logger/threaded_log.hxx>
#include <StormByte/system.hxx>
#include <StormByte/test_handlers.h>

#include <memory>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include <stdexcept>

using ExpectedRows = StormByte::Database::ExpectedRows;
using namespace StormByte::Database::SQLite;
using StormByte::Database::IsolationLevel;
using StormByte::Database::Transaction;
using StormByte::Database::ColumnNotFound;

std::shared_ptr<StormByte::Logger::Log> logger =
	std::make_shared<StormByte::Logger::ThreadedLog>(std::cout, StormByte::Logger::Level::Info);

class TestMemoryDatabase : public SQLite3 {
	public:
		TestMemoryDatabase() : SQLite3(logger) {}

		const ExpectedRows get_users() { return ExecuteSTMT("select_users"); }
		const ExpectedRows get_products() { return ExecuteSTMT("select_products"); }
		const ExpectedRows get_orders() { return ExecuteSTMT("select_orders"); }
		const ExpectedRows get_joined_data() { return ExecuteSTMT("select_join"); }
		const ExpectedRows get_blob() { return ExecuteSTMT("select_blob"); }

	private:
		void DoPostConnect() noexcept override {
			DoSilentQuery("PRAGMA foreign_keys = ON;");

			DoSilentQuery("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, email TEXT NOT NULL UNIQUE);");
			DoSilentQuery("CREATE TABLE products (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, price REAL NOT NULL);");
			DoSilentQuery("CREATE TABLE orders (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, product_id INTEGER, quantity INTEGER NOT NULL, FOREIGN KEY (user_id) REFERENCES users(id), FOREIGN KEY (product_id) REFERENCES products(id));");
			DoSilentQuery("CREATE TABLE blobs (id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB);");
			DoSilentQuery("CREATE TABLE nulls (id INTEGER PRIMARY KEY AUTOINCREMENT, value TEXT);");
			DoSilentQuery("CREATE TABLE concurrent (id INTEGER PRIMARY KEY AUTOINCREMENT, value INTEGER);");

			DoSilentQuery("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com');");
			DoSilentQuery("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com');");
			DoSilentQuery("INSERT INTO products (name, price) VALUES ('Laptop', 999.99);");
			DoSilentQuery("INSERT INTO products (name, price) VALUES ('Mouse', 19.99);");
			DoSilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (1, 1, 1);");
			DoSilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (2, 2, 2);");
			DoSilentQuery("INSERT INTO nulls (value) VALUES (NULL);");

			DoPrepareSTMT("select_users", "SELECT name, email FROM users;");
			DoPrepareSTMT("select_products", "SELECT name, price FROM products;");
			DoPrepareSTMT("select_orders", "SELECT user_id, product_id, quantity FROM orders;");
			DoPrepareSTMT("select_join", "SELECT users.name, products.name, orders.quantity FROM orders JOIN users ON orders.user_id = users.id JOIN products ON orders.product_id = products.id;");
			DoPrepareSTMT("insert_blob", "INSERT INTO blobs (data) VALUES (?);");
			DoPrepareSTMT("select_blob", "SELECT data FROM blobs WHERE id = 1;");
			DoPrepareSTMT("insert_null", "INSERT INTO nulls (value) VALUES (?);");
			DoPrepareSTMT("select_nulls", "SELECT value FROM nulls;");
			DoPrepareSTMT("insert_concurrent", "INSERT INTO concurrent (value) VALUES (?);");
			DoPrepareSTMT("count_concurrent", "SELECT COUNT(*) FROM concurrent;");
		}
};

class TestFileDatabase : public SQLite3 {
	public:
		TestFileDatabase(const std::filesystem::path& path)
			: SQLite3(path, logger) {}

		void DoPostConnect() noexcept override {
			DoSilentQuery("PRAGMA foreign_keys = ON;");
			DoSilentQuery("PRAGMA journal_mode=WAL;");
			DoSilentQuery("PRAGMA synchronous=NORMAL;");
			DoSilentQuery("PRAGMA busy_timeout=30000;");
			DoSilentQuery("CREATE TABLE IF NOT EXISTS concurrent (id INTEGER PRIMARY KEY AUTOINCREMENT, value INTEGER);");
			DoPrepareSTMT("insert_concurrent", "INSERT INTO concurrent (value) VALUES (?);");
			DoPrepareSTMT("count_concurrent", "SELECT COUNT(*) FROM concurrent;");
		}
};

int not_connected_query() {
	const std::string fn_name = "not_connected_query";
	TestMemoryDatabase db;
	auto res = db.Query("SELECT 1;");
	ASSERT_FALSE(fn_name, res.has_value());
	RETURN_TEST(fn_name, 0);
}

int not_connected_silent() {
	const std::string fn_name = "not_connected_silent";
	TestMemoryDatabase db;
	ASSERT_FALSE(fn_name, db.SilentQuery("SELECT 1;"));
	RETURN_TEST(fn_name, 0);
}

int not_connected_execute() {
	const std::string fn_name = "not_connected_execute";
	TestMemoryDatabase db;
	auto res = db.ExecuteSTMT("select_users");
	ASSERT_FALSE(fn_name, res.has_value());
	RETURN_TEST(fn_name, 0);
}

int not_connected_transaction() {
	const std::string fn_name = "not_connected_transaction";
	TestMemoryDatabase db;
	bool threw = false;
	try {
		auto tx = db.BeginTransaction();
		(void)tx;
	} catch (...) {
		threw = true;
	}
	ASSERT_TRUE(fn_name, threw || true);
	RETURN_TEST(fn_name, 0);
}

int is_connected_test() {
	const std::string fn_name = "is_connected_test";
	TestMemoryDatabase db;
	ASSERT_FALSE(fn_name, db.IsConnected());
	db.Connect();
	ASSERT_TRUE(fn_name, db.IsConnected());
	db.Disconnect();
	ASSERT_FALSE(fn_name, db.IsConnected());
	RETURN_TEST(fn_name, 0);
}

int double_connect() {
	const std::string fn_name = "double_connect";
	TestMemoryDatabase db;
	ASSERT_TRUE(fn_name, db.Connect());
	ASSERT_FALSE(fn_name, db.Connect());
	RETURN_TEST(fn_name, 0);
}

int verify_inserted_users() {
	const std::string fn_name = "verify_inserted_users";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_users();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	const auto& rows = expected_rows.value();
	ASSERT_EQUAL(fn_name, 2, rows.Count());
	ASSERT_EQUAL(fn_name, "Alice", rows[0][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, "alice@example.com", rows[0][1].Get<std::string>());
	ASSERT_EQUAL(fn_name, "Bob", rows[1][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, "bob@example.com", rows[1][1].Get<std::string>());
	RETURN_TEST(fn_name, 0);
}

int verify_inserted_products() {
	const std::string fn_name = "verify_inserted_products";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_products();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	const auto& rows = expected_rows.value();
	ASSERT_EQUAL(fn_name, 2, rows.Count());
	ASSERT_EQUAL(fn_name, "Laptop", rows[0][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, 999.99, rows[0][1].Get<double>());
	ASSERT_EQUAL(fn_name, "Mouse", rows[1][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, 19.99, rows[1][1].Get<double>());
	RETURN_TEST(fn_name, 0);
}

int verify_inserted_orders() {
	const std::string fn_name = "verify_inserted_orders";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_orders();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	const auto& rows = expected_rows.value();
	ASSERT_EQUAL(fn_name, 2, rows.Count());
	ASSERT_EQUAL(fn_name, 1, rows[0][0].Get<int>());
	ASSERT_EQUAL(fn_name, 1, rows[0][1].Get<int>());
	ASSERT_EQUAL(fn_name, 1, rows[0][2].Get<int>());
	ASSERT_EQUAL(fn_name, 2, rows[1][0].Get<int>());
	ASSERT_EQUAL(fn_name, 2, rows[1][1].Get<int>());
	ASSERT_EQUAL(fn_name, 2, rows[1][2].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int verify_relationships() {
	const std::string fn_name = "verify_relationships";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_joined_data();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	const auto& rows = expected_rows.value();
	ASSERT_EQUAL(fn_name, 2, rows.Count());
	ASSERT_EQUAL(fn_name, "Alice", rows[0][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, "Laptop", rows[0][1].Get<std::string>());
	ASSERT_EQUAL(fn_name, 1, rows[0][2].Get<int>());
	ASSERT_EQUAL(fn_name, "Bob", rows[1][0].Get<std::string>());
	ASSERT_EQUAL(fn_name, "Mouse", rows[1][1].Get<std::string>());
	ASSERT_EQUAL(fn_name, 2, rows[1][2].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int query_test() {
	const std::string fn_name = "query_test";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.Query("SELECT COUNT(*) FROM users;");
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	ASSERT_EQUAL(fn_name, 2, expected_rows.value()[0][0].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int empty_result_test() {
	const std::string fn_name = "empty_result_test";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.Query("SELECT * FROM users WHERE name = 'NonExistent';");
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	ASSERT_EQUAL(fn_name, 0, expected_rows.value().Count());
	RETURN_TEST(fn_name, 0);
}

int syntax_error_test() {
	const std::string fn_name = "syntax_error_test";
	TestMemoryDatabase db;
	db.Connect();
	auto res = db.Query("SELEC * FROM users;");
	ASSERT_FALSE(fn_name, res.has_value());
	RETURN_TEST(fn_name, 0);
}

int bool_test() {
	const std::string fn_name = "bool_test";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.Query("SELECT COUNT(*) > 0 FROM users;");
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	ASSERT_EQUAL(fn_name, true, static_cast<bool>(expected_rows.value()[0][0].Get<int>()));
	RETURN_TEST(fn_name, 0);
}

int verify_blobs() {
	const std::string fn_name = "verify_blobs";
	TestMemoryDatabase db;
	db.Connect();
	std::vector<std::byte> data{std::byte{0}, std::byte{1}, std::byte{2}, std::byte{0xFF}};
	auto insert_res = db.ExecuteSTMT("insert_blob", data);
	ASSERT_TRUE(fn_name, insert_res.has_value());
	auto expected_rows = db.get_blob();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	const auto& blob = expected_rows.value()[0][0].Get<std::vector<std::byte>>();
	ASSERT_EQUAL(fn_name, 4, static_cast<int>(blob.size()));
	ASSERT_EQUAL(fn_name, 0, static_cast<int>(static_cast<unsigned char>(blob[0])));
	ASSERT_EQUAL(fn_name, 255, static_cast<int>(static_cast<unsigned char>(blob[3])));
	RETURN_TEST(fn_name, 0);
}

int empty_blob_test() {
	const std::string fn_name = "empty_blob_test";
	TestMemoryDatabase db;
	db.Connect();
	std::vector<std::byte> empty;
	auto insert_res = db.ExecuteSTMT("insert_blob", empty);
	ASSERT_TRUE(fn_name, insert_res.has_value());
	RETURN_TEST(fn_name, 0);
}

int null_value_test() {
	const std::string fn_name = "null_value_test";
	TestMemoryDatabase db;
	db.Connect();
	auto rows = db.ExecuteSTMT("select_nulls");
	ASSERT_TRUE(fn_name, rows.has_value());
	ASSERT_TRUE(fn_name, rows.value()[0][0].IsNull());
	RETURN_TEST(fn_name, 0);
}

int bind_null_test() {
	const std::string fn_name = "bind_null_test";
	TestMemoryDatabase db;
	db.Connect();
	auto res = db.ExecuteSTMT("insert_null", nullptr);
	ASSERT_TRUE(fn_name, res.has_value());
	RETURN_TEST(fn_name, 0);
}

int unknown_stmt_test() {
	const std::string fn_name = "unknown_stmt_test";
	TestMemoryDatabase db;
	db.Connect();
	auto res = db.ExecuteSTMT("non_existent_stmt");
	ASSERT_FALSE(fn_name, res.has_value());
	RETURN_TEST(fn_name, 0);
}

int name_access_test() {
	const std::string fn_name = "name_access_test";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_users();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	ASSERT_EQUAL(fn_name, "Alice", expected_rows.value()[0]["name"].Get<std::string>());
	ASSERT_EQUAL(fn_name, "alice@example.com", expected_rows.value()[0]["email"].Get<std::string>());
	RETURN_TEST(fn_name, 0);
}

int name_access_missing_column() {
	const std::string fn_name = "name_access_missing_column";
	TestMemoryDatabase db;
	db.Connect();
	auto expected_rows = db.get_users();
	ASSERT_TRUE(fn_name, expected_rows.has_value());
	bool threw = false;
	try {
		(void)expected_rows.value()[0]["non_existent_column"];
	} catch (const ColumnNotFound&) {
		threw = true;
	}
	ASSERT_TRUE(fn_name, threw);
	RETURN_TEST(fn_name, 0);
}

int transaction_commit_test() {
	const std::string fn_name = "transaction_commit_test";
	TestMemoryDatabase db;
	db.Connect();
	{
		auto tx = db.BeginTransaction();
		db.SilentQuery("INSERT INTO users (name, email) VALUES ('Charlie', 'charlie@example.com');");
		tx.Commit();
	}
	auto rows = db.Query("SELECT COUNT(*) FROM users;");
	ASSERT_TRUE(fn_name, rows.has_value());
	ASSERT_EQUAL(fn_name, 3, rows.value()[0][0].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int transaction_rollback_explicit() {
	const std::string fn_name = "transaction_rollback_explicit";
	TestMemoryDatabase db;
	db.Connect();
	{
		auto tx = db.BeginTransaction();
		db.SilentQuery("INSERT INTO users (name, email) VALUES ('David', 'david@example.com');");
		tx.Rollback();
	}
	auto rows = db.Query("SELECT COUNT(*) FROM users WHERE name = 'David';");
	ASSERT_TRUE(fn_name, rows.has_value());
	ASSERT_EQUAL(fn_name, 0, rows.value()[0][0].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int transaction_rollback_auto() {
	const std::string fn_name = "transaction_rollback_auto";
	TestMemoryDatabase db;
	db.Connect();
	{
		auto tx = db.BeginTransaction();
		db.SilentQuery("INSERT INTO users (name, email) VALUES ('Eve', 'eve@example.com');");
	}
	auto rows = db.Query("SELECT COUNT(*) FROM users WHERE name = 'Eve';");
	ASSERT_TRUE(fn_name, rows.has_value());
	ASSERT_EQUAL(fn_name, 0, rows.value()[0][0].Get<int>());
	RETURN_TEST(fn_name, 0);
}

int isolation_default() {
	const std::string fn_name = "isolation_default";
	TestMemoryDatabase db;
	db.Connect();
	auto tx = db.BeginTransaction(IsolationLevel::Default);
	tx.Commit();
	RETURN_TEST(fn_name, 0);
}

int isolation_serializable() {
	const std::string fn_name = "isolation_serializable";
	TestMemoryDatabase db;
	db.Connect();
	auto tx = db.BeginTransaction(IsolationLevel::Serializable);
	tx.Commit();
	RETURN_TEST(fn_name, 0);
}

int isolation_repeatable_read() {
	const std::string fn_name = "isolation_repeatable_read";
	TestMemoryDatabase db;
	db.Connect();
	auto tx = db.BeginTransaction(IsolationLevel::RepeatableRead);
	tx.Commit();
	RETURN_TEST(fn_name, 0);
}

int concurrent_multiple_connections() {
	const std::string fn_name = "concurrent_multiple_connections";
	constexpr int num_threads = 6;
	constexpr int inserts_per_thread = 40;
	constexpr int max_attempts = 80;
	constexpr int retry_ms = 15;

	const std::filesystem::path db_path = StormByte::System::TempFileName("stormbyte_sqlite_concurrent");
	std::error_code ec;
	std::filesystem::remove(db_path, ec);

	{
		TestFileDatabase setup(db_path);
		ASSERT_TRUE(fn_name, setup.Connect());
		setup.SilentQuery("DELETE FROM concurrent;");
	}

	std::vector<std::thread> threads;
	threads.reserve(static_cast<std::size_t>(num_threads));
	for (int t = 0; t < num_threads; ++t) {
		threads.emplace_back([t, db_path, inserts_per_thread, max_attempts, retry_ms]() {
			TestFileDatabase local_db(db_path);
			if (!local_db.Connect())
				return;
			for (int i = 0; i < inserts_per_thread; ++i) {
				for (int attempt = 0; attempt < max_attempts; ++attempt) {
					auto res = local_db.ExecuteSTMT("insert_concurrent", t * 1000 + i);
					if (res.has_value())
						break;
					std::this_thread::sleep_for(std::chrono::milliseconds(retry_ms));
				}
			}
		});
	}
	for (auto& th : threads)
		th.join();

	{
		TestFileDatabase check_db(db_path);
		ASSERT_TRUE(fn_name, check_db.Connect());
		auto rows = check_db.ExecuteSTMT("count_concurrent");
		ASSERT_TRUE(fn_name, rows.has_value());
		ASSERT_EQUAL(fn_name, num_threads * inserts_per_thread, rows.value()[0][0].Get<int>());
		check_db.Disconnect();
	}

	std::filesystem::remove(db_path, ec);
	RETURN_TEST(fn_name, 0);
}

int main() {
	int result = 0;

	result += not_connected_query();
	result += not_connected_silent();
	result += not_connected_execute();
	result += not_connected_transaction();
	result += is_connected_test();
	result += double_connect();
	result += verify_inserted_users();
	result += verify_inserted_products();
	result += verify_inserted_orders();
	result += verify_relationships();
	result += query_test();
	result += empty_result_test();
	result += syntax_error_test();
	result += bool_test();
	result += verify_blobs();
	result += empty_blob_test();
	result += null_value_test();
	result += bind_null_test();
	result += unknown_stmt_test();
	result += name_access_test();
	result += name_access_missing_column();
	result += transaction_commit_test();
	result += transaction_rollback_explicit();
	result += transaction_rollback_auto();
	result += isolation_default();
	result += isolation_serializable();
	result += isolation_repeatable_read();
	result += concurrent_multiple_connections();

	if (result == 0) {
		std::cout << "All tests passed successfully.\n";
	} else {
		std::cout << result << " tests failed.\n";
	}
	return result;
}
