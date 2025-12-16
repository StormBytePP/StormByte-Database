#include <StormByte/database/mariadb/mariadb.hxx>
#include <StormByte/logger/log.hxx>
#include <StormByte/system.hxx>
#include <StormByte/test_handlers.h>

#include <memory>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

using ExpectedRows = StormByte::Database::ExpectedRows;
using namespace StormByte::Database::MariaDB;

std::shared_ptr<StormByte::Logger::Log> logger = std::make_shared<StormByte::Logger::Log>(std::cout, StormByte::Logger::Level::Info);

class TestDatabase : public MariaDB {
    public:
        TestDatabase(): MariaDB(
            "127.0.0.1",
            "testuser",
            "testpass",
            "stormbyte_test",
            3306,
            logger
        ) {}

        const ExpectedRows get_users() { return ExecuteSTMT("select_users"); }
        const ExpectedRows get_products() { return ExecuteSTMT("select_products"); }
        const ExpectedRows get_orders() { return ExecuteSTMT("select_orders"); }
        const ExpectedRows get_joined_data() { return ExecuteSTMT("select_join"); }
        const ExpectedRows get_blob() { return ExecuteSTMT("select_blob"); }

    private:
        void DoPostConnect() noexcept override {
            // Create tables (MariaDB/MySQL dialect)
            SilentQuery("CREATE TABLE IF NOT EXISTS users (id INT PRIMARY KEY AUTO_INCREMENT, name TEXT NOT NULL, email TEXT NOT NULL UNIQUE);");
            SilentQuery("CREATE TABLE IF NOT EXISTS products (id INT PRIMARY KEY AUTO_INCREMENT, name TEXT NOT NULL, price DOUBLE NOT NULL);");
            SilentQuery("CREATE TABLE IF NOT EXISTS orders (id INT PRIMARY KEY AUTO_INCREMENT, user_id INT, product_id INT, quantity INT NOT NULL, FOREIGN KEY (user_id) REFERENCES users(id), FOREIGN KEY (product_id) REFERENCES products(id));");
            SilentQuery("CREATE TABLE IF NOT EXISTS blobs (id INT PRIMARY KEY AUTO_INCREMENT, data BLOB);");

            // Ensure a clean slate for tests without disabling foreign-key checks
            SilentQuery("DELETE FROM orders;");
            SilentQuery("DELETE FROM blobs;");
            SilentQuery("DELETE FROM users;");
            SilentQuery("DELETE FROM products;");
            // Reset AUTO_INCREMENT so IDs start from 1
            SilentQuery("ALTER TABLE orders AUTO_INCREMENT=1;");
            SilentQuery("ALTER TABLE blobs AUTO_INCREMENT=1;");
            SilentQuery("ALTER TABLE users AUTO_INCREMENT=1;");
            SilentQuery("ALTER TABLE products AUTO_INCREMENT=1;");
            SilentQuery("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com');");
            SilentQuery("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com');");
            SilentQuery("INSERT INTO products (name, price) VALUES ('Laptop', 999.99);");
            SilentQuery("INSERT INTO products (name, price) VALUES ('Mouse', 19.99);");
            SilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (1, 1, 1);");
            SilentQuery("INSERT INTO orders (user_id, product_id, quantity) VALUES (2, 2, 2);");

            // Prepare statements (use ? placeholders for MariaDB)
            PrepareSTMT("select_users", "SELECT name, email FROM users;");
            PrepareSTMT("select_products", "SELECT name, price FROM products;");
            PrepareSTMT("select_orders", "SELECT user_id, product_id, quantity FROM orders;");
            PrepareSTMT("select_join", "SELECT users.name, products.name, orders.quantity FROM orders JOIN users ON orders.user_id = users.id JOIN products ON orders.product_id = products.id;");

            // Blob table for testing
            PrepareSTMT("insert_blob", "INSERT INTO blobs (data) VALUES (?);");
            PrepareSTMT("select_blob", "SELECT data FROM blobs WHERE id = 1;");
        }
};

int verify_inserted_users() {
    const std::string fn_name = "verify_inserted_users";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.get_users();
    ASSERT_TRUE(fn_name , expected_rows.has_value());

    const auto& rows = expected_rows.value();
    
    ASSERT_EQUAL(fn_name , 2, rows.Count());
    ASSERT_EQUAL(fn_name, 2, rows[0].Count());
    ASSERT_EQUAL(fn_name , "Alice", rows[0][0].Get<std::string>());
    ASSERT_EQUAL(fn_name , "alice@example.com", rows[0][1].Get<std::string>());
    ASSERT_EQUAL(fn_name, 2, rows[1].Count());
    ASSERT_EQUAL(fn_name , "Bob", rows[1][0].Get<std::string>());
    ASSERT_EQUAL(fn_name , "bob@example.com", rows[1][1].Get<std::string>());

    RETURN_TEST(fn_name , 0);
}

int verify_inserted_products() {
    const std::string fn_name = "verify_inserted_products";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.get_products();
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto& rows = expected_rows.value();
    ASSERT_EQUAL(fn_name, 2, rows.Count());
    ASSERT_EQUAL(fn_name, 2, rows[0].Count());
    ASSERT_EQUAL(fn_name, "Laptop", rows[0][0].Get<std::string>());
    ASSERT_EQUAL(fn_name, 999.99, rows[0][1].Get<double>());
    ASSERT_EQUAL(fn_name, 2, rows[1].Count());
    ASSERT_EQUAL(fn_name, "Mouse", rows[1][0].Get<std::string>());
    ASSERT_EQUAL(fn_name, 19.99, rows[1][1].Get<double>());

    RETURN_TEST(fn_name, 0);
}

int verify_inserted_orders() {
    const std::string fn_name = "verify_inserted_orders";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.get_orders();
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto rows = expected_rows.value();
    
    ASSERT_EQUAL(fn_name, 2, rows.Count());
    ASSERT_EQUAL(fn_name, 3, rows[0].Count());
    ASSERT_EQUAL(fn_name, 1, rows[0][0].Get<long int>());
    ASSERT_EQUAL(fn_name, 1, rows[0][1].Get<long int>());
    ASSERT_EQUAL(fn_name, 1, rows[0][2].Get<long int>());
    ASSERT_EQUAL(fn_name, 3, rows[1].Count());
    ASSERT_EQUAL(fn_name, 2, rows[1][0].Get<long int>());
    ASSERT_EQUAL(fn_name, 2, rows[1][1].Get<long int>());
    ASSERT_EQUAL(fn_name, 2, rows[1][2].Get<long int>());

    RETURN_TEST(fn_name , 0);
}

int verify_relationships() {
    const std::string fn_name = "verify_relationships";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.get_joined_data();
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto& rows = expected_rows.value();
    ASSERT_EQUAL(fn_name, 2, rows.Count());
    ASSERT_EQUAL(fn_name, 3, rows[0].Count());
    ASSERT_EQUAL(fn_name, "Alice", rows[0][0].Get<std::string>());
    ASSERT_EQUAL(fn_name, "Laptop", rows[0][1].Get<std::string>());
    ASSERT_EQUAL(fn_name, 1, rows[0][2].Get<long int>());
    ASSERT_EQUAL(fn_name, 3, rows[1].Count());
    ASSERT_EQUAL(fn_name, "Bob", rows[1][0].Get<std::string>());
    ASSERT_EQUAL(fn_name, "Mouse", rows[1][1].Get<std::string>());
    ASSERT_EQUAL(fn_name, 2, rows[1][2].Get<long int>());

    RETURN_TEST(fn_name , 0);
}

int query_test() {
    const std::string fn_name = "query_test";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.Query("SELECT COUNT(*) FROM users;");
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto& rows = expected_rows.value();
    ASSERT_EQUAL(fn_name, 1, rows.Count());
    ASSERT_EQUAL(fn_name, 1, rows[0].Count());
    ASSERT_EQUAL(fn_name, 2, rows[0][0].Get<long int>());

    RETURN_TEST(fn_name, 0);
}

int bool_test() {
    const std::string fn_name = "bool_test";
    TestDatabase db;
    db.Connect();

    auto expected_rows = db.Query("SELECT COUNT(*) > 0 FROM users;");
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto& rows = expected_rows.value();
    ASSERT_EQUAL(fn_name, 1, rows.Count());
    ASSERT_EQUAL(fn_name, 1, rows[0].Count());
    ASSERT_EQUAL(fn_name, true, rows[0][0].Get<long int>() != 0);

    RETURN_TEST(fn_name, 0);
}

int verify_blobs() {
    const std::string fn_name = "verify_blobs";
    TestDatabase db;
    db.Connect();

    std::vector<std::byte> data;
    data.push_back(std::byte{0});
    data.push_back(std::byte{1});
    data.push_back(std::byte{2});
    data.push_back(static_cast<std::byte>(0xFF));

    auto insert_res = db.ExecuteSTMT("insert_blob", data);
    ASSERT_TRUE(fn_name, insert_res.has_value());

    auto expected_rows = db.get_blob();
    ASSERT_TRUE(fn_name, expected_rows.has_value());
    const auto& rows = expected_rows.value();
    ASSERT_EQUAL(fn_name, 1, rows.Count());
    ASSERT_EQUAL(fn_name, 1, rows[0].Count());

    const auto& blob = rows[0][0].Get<std::vector<std::byte>>();
    ASSERT_EQUAL(fn_name, 4, static_cast<int>(blob.size()));
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(blob.data());
    ASSERT_EQUAL(fn_name, 0, static_cast<int>(bytes[0]));
    ASSERT_EQUAL(fn_name, 1, static_cast<int>(bytes[1]));
    ASSERT_EQUAL(fn_name, 2, static_cast<int>(bytes[2]));
    ASSERT_EQUAL(fn_name, 255, static_cast<int>(bytes[3]));

    RETURN_TEST(fn_name, 0);
}

int main() {
    int result = 0;

    result += verify_inserted_users();
    result += verify_inserted_products();
    result += verify_inserted_orders();
    result += verify_relationships();
    result += query_test();
    result += bool_test();
    result += verify_blobs();

    if (result == 0) {
        std::cout << "All tests passed successfully.\n";
    } else {
        std::cout << result << " tests failed.\n";
    }

    return result;
}
