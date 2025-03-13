#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/util/system.hxx>
#include <StormByte/test_handlers.h>

#include <memory>
#include <iostream>
#include <vector>

using namespace StormByte::Database::SQLite;

class TestMemoryDatabase : public SQLite3 {
	public:
		TestMemoryDatabase() : SQLite3() {
			post_init_action();
		}
	
		void post_init_action() noexcept {
			// Create tables
			SilentQuery("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, email TEXT NOT NULL UNIQUE);");
			SilentQuery("CREATE TABLE products (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, price REAL NOT NULL);");
			SilentQuery("CREATE TABLE orders (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, product_id INTEGER, quantity INTEGER NOT NULL, FOREIGN KEY (user_id) REFERENCES users(id), FOREIGN KEY (product_id) REFERENCES products(id));");
	
			// Insert data
			SilentQuery("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com');");
			SilentQuery("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com');");
			SilentQuery("INSERT INTO products (name, price) VALUES ('Laptop', 999.99);");
			SilentQuery("INSERT INTO products (name, price) VALUES ('Mouse', 19.99);");
			SilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (1, 1, 1);");
			SilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (2, 2, 2);");

			// Prepare statements
			PrepareSTMT("select_users", "SELECT name, email FROM users;");
			PrepareSTMT("select_products", "SELECT name, price FROM products;");
			PrepareSTMT("select_orders", "SELECT user_id, product_id, quantity FROM orders;");
			PrepareSTMT("select_join", "SELECT users.name, products.name, orders.quantity FROM orders JOIN users ON orders.user_id = users.id JOIN products ON orders.product_id = products.id;");
		}

		const std::vector<Row> get_users() const {
			auto& stmt = GetPreparedSTMT("select_users");
			std::vector<Row> rows;
			while (const Row& r = stmt.Step())
				rows.push_back(r);
			return rows;
		}
	
		const std::vector<Row> get_products() const {
			auto& stmt = GetPreparedSTMT("select_products");
			std::vector<Row> rows;
			while (const Row& r = stmt.Step())
				rows.push_back(r);
			return rows;
		}
	
		const std::vector<Row> get_orders() const {
			auto& stmt = GetPreparedSTMT("select_orders");
			std::vector<Row> rows;
			while (const Row& r = stmt.Step())
				rows.push_back(r);
			return rows;
		}
	
		const std::vector<Row> get_joined_data() const {
			auto& stmt = GetPreparedSTMT("select_join");
			std::vector<Row> rows;
			while (const Row& r = stmt.Step())
				rows.push_back(r);
			return rows;
		}
};

class TestFileDatabase : public SQLite3 {
	public:
		TestFileDatabase() : SQLite3(CurrentFileDirectory / "files" / "test.db") {
			post_init_action();
		}
	
		void post_init_action() noexcept {
			PrepareSTMT("select_users", "SELECT name, email FROM users;");
		}

		const std::vector<Row> get_users() const {
			auto& stmt = GetPreparedSTMT("select_users");
			std::vector<Row> rows;
			while (const Row& r = stmt.Step())
				rows.push_back(r);
			return rows;
		}
};

int verify_inserted_users() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 1. Verify that users were inserted correctly
	auto rows = db.get_users();
	ASSERT_EQUAL("verify_inserted_users", 2, rows[0].Columns());
	ASSERT_EQUAL("verify_inserted_users", "Alice", rows[0][0].Get<std::string>());
	ASSERT_EQUAL("verify_inserted_users", "alice@example.com", rows[0][1].Get<std::string>());

	ASSERT_EQUAL("verify_inserted_users", 2, rows[1].Columns());
    ASSERT_EQUAL("verify_inserted_users", "Bob", rows[1][0].Get<std::string>());
    ASSERT_EQUAL("verify_inserted_users", "bob@example.com", rows[1][1].Get<std::string>());

	return 0;
}

int verify_inserted_products() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 2. Verify that products were inserted correctly
	auto rows = db.get_products();
	ASSERT_EQUAL("verify_inserted_products", 2, rows[0].Columns());
	ASSERT_EQUAL("verify_inserted_products", "Laptop", rows[0][0].Get<std::string>());
	ASSERT_EQUAL("verify_inserted_products", 999.99, rows[0][1].Get<double>());

	ASSERT_EQUAL("verify_inserted_products", 2, rows[1].Columns());
	ASSERT_EQUAL("verify_inserted_products", "Mouse", rows[1][0].Get<std::string>());
	ASSERT_EQUAL("verify_inserted_products", 19.99, rows[1][1].Get<double>());

	return 0;
}

int verify_inserted_orders() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 3. Verify that orders were inserted correctly
	auto rows = db.get_orders();
	ASSERT_EQUAL("verify_inserted_orders", 3, rows[0].Columns());
	ASSERT_EQUAL("verify_inserted_orders", 1, rows[0][0].Get<int>());
	ASSERT_EQUAL("verify_inserted_orders", 1, rows[0][1].Get<int>());
	ASSERT_EQUAL("verify_inserted_orders", 1, rows[0][2].Get<int>());

	ASSERT_EQUAL("verify_inserted_orders", 3, rows[1].Columns());
	ASSERT_EQUAL("verify_inserted_orders", 2, rows[1][0].Get<int>());
	ASSERT_EQUAL("verify_inserted_orders", 2, rows[1][1].Get<int>());
	ASSERT_EQUAL("verify_inserted_orders", 2, rows[1][2].Get<int>());

	return 0;
}

int verify_relationships() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 4. Verify that the relationship between tables works correctly
	auto rows = db.get_joined_data();
	ASSERT_EQUAL("verify_relationships", 3, rows[0].Columns());
	ASSERT_EQUAL("verify_relationships", "Alice", rows[0][0].Get<std::string>());
	ASSERT_EQUAL("verify_relationships", "Laptop", rows[0][1].Get<std::string>());
	ASSERT_EQUAL("verify_relationships", 1, rows[0][2].Get<int>());

	ASSERT_EQUAL("verify_relationships", 3, rows[1].Columns());
	ASSERT_EQUAL("verify_relationships", "Bob", rows[1][0].Get<std::string>());
	ASSERT_EQUAL("verify_relationships", "Mouse", rows[1][1].Get<std::string>());
	ASSERT_EQUAL("verify_relationships", 2, rows[1][2].Get<int>());

	return 0;
}

int query_test() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 5. Verify that the query method works correctly
	auto query = db.PrepareQuery("SELECT COUNT(*) FROM users;");
	auto row = query->Step();
	ASSERT_EQUAL("query_test", 1, row.Columns());
	ASSERT_EQUAL("query_test", 2, row[0].Get<int>());

	RETURN_TEST("query_test", 0);
}

int bool_test() {
	// Create an in-memory database for testing
	TestMemoryDatabase db;

	// 6. Verify that the query method works correctly
	auto query = db.PrepareQuery("SELECT COUNT(*) > 0 FROM users;");
	auto row = query->Step();
	ASSERT_EQUAL("bool_test", 1, row.Columns());
	ASSERT_EQUAL("bool_test", true, (bool)row[0].Get<int>());

	RETURN_TEST("bool_test", 0);
}

int read_from_binary_database() {
	// Create an in-memory database for testing
	TestFileDatabase db;

	// 1. Verify that users were inserted correctly
	auto rows = db.get_users();
	ASSERT_EQUAL("verify_inserted_users", 2, rows[0].Columns());
	ASSERT_EQUAL("verify_inserted_users", "John Doe", rows[0][0].Get<std::string>());
	ASSERT_EQUAL("verify_inserted_users", "john@example.com", rows[0][1].Get<std::string>());

	ASSERT_EQUAL("verify_inserted_users", 2, rows[1].Columns());
    ASSERT_EQUAL("verify_inserted_users", "Sarah Connor", rows[1][0].Get<std::string>());
    ASSERT_EQUAL("verify_inserted_users", "sarah@example.com", rows[1][1].Get<std::string>());

	return 0;
}

int main() {
    int result = 0;
    try {
		//result += verify_inserted_users();
		//result += verify_inserted_products();
		//result += verify_inserted_orders();
		//result += verify_relationships();
		//result += query_test();
		//result += bool_test();
		result += read_from_binary_database();
        std::cout << "All tests passed successfully.\n";
    } catch (const StormByte::Database::SQLite::Exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
        result++;
	}

    return result;
}
