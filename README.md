# StormByte

StormByte is a comprehensive, cross-platform C++ library aimed at easing system programming, configuration management, logging, and database handling tasks. This library provides a unified API that abstracts away the complexities and inconsistencies of different platforms (Windows, Linux).

## Features

- **Database Handling**: Includes SQLite support for embedded database management while hiding SQLite3 internals conveniently.

## Table of Contents

- [Repository](#Repository)
- [Installation](#Installation)
- [Modules](#Modules)
	- [Base](https://dev.stormbyte.org/StormByte)
	- [Config](https://dev.stormbyte.org/StormByte-Config)
	- **Database**
	- [Multimedia](https://dev.stormbyte.org/StormByte-Multimedia)
	- [Network](https://dev.stormbyte.org/StormByte-Network)
	- [System](https://dev.stormbyte.org/StormByte-System)
- [Contributing](#Contributing)
- [License](#License)

## Repository

You can visit the code repository at [GitHub](https://github.com/StormBytePP/StormByte-Database)

## Installation

### Prerequisites

Ensure you have the following installed:

- C++23 compatible compiler
- CMake 3.12 or higher
- SQLite3 for database backend support (optional)

### Building

To build the library, follow these steps:

```sh
git clone https://github.com/StormBytePP/StormByte-Database.git
cd StormByte-Database
mkdir build
cd build
cmake ..
make
```

## Modules

StormByte Library is composed by several modules:

### Database

The `Database` module provides support for SQLite, an embedded SQL database engine. It includes classes for managing database connections, prepared statements, and result rows.

#### Example: Database

```cpp
#include <StormByte/database/sqlite/sqlite3.hxx>
#include <memory>
#include <iostream>

class MyDatabase : public StormByte::Database::SQLite::SQLite3 {
public:
	MyDatabase(const std::filesystem::path& dbfile) : SQLite3(dbfile) {
		init_database();
	}

	void print_all_users() {
		auto stmt = prepare_select_all_users();
		while (auto row = stmt->Step()) {
			std::cout << "ID: " << row->At(0)->Value<int>() << " Name: " << row->At(1)->Value<std::string>() << std::endl;
		}
	}

protected:
	void post_init_action() noexcept override {
		try {
			silent_query("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT NOT NULL)");
		} catch (const StormByte::Database::SQLite::Exception& e) {
			std::cerr << "Database initialization error: " << e.what() << std::endl;
		}
	}

	std::shared_ptr<StormByte::Database::SQLite::PreparedSTMT> prepare_select_all_users() {
		return prepare_sentence("select_all_users", "SELECT * FROM users");
	}
};

// Example usage
int main() {
	MyDatabase db("/path/to/database.db");
	db.print_all_users();
	return 0;
}
```

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.

## License

This project is licensed under GPL v3 License - see the [LICENSE](LICENSE) file for details.
