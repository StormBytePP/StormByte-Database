# StormByte
![Linux](https://img.shields.io/badge/Linux-Supported-1793D1?logo=linux&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-Supported-0078D6?logo=windows&logoColor=white)
![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.12+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)
![Status](https://img.shields.io/badge/Status-Active-success)
[![Compile & Test](https://github.com/StormBytePP/StormByte-Database/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/StormBytePP/StormByte-Database/actions/workflows/build.yml)

StormByte is a comprehensive, cross-platform C++ library aimed at easing system programming, configuration management, logging, and database handling tasks. This library provides a unified API that abstracts away the complexities and inconsistencies of different platforms (Windows, Linux).

## Features

- **Database Handling**: Includes SQLite support for embedded database management while hiding SQLite3 internals conveniently.

## Table of Contents

- [Repository](#Repository)
- [Installation](#Installation)
- [Modules](#Modules)
	- [Base](https://dev.stormbyte.org/StormByte)
	- [Buffer](https://dev.stormbyte.org/StormByte-Buffer)
	- [Config](https://dev.stormbyte.org/StormByte-Config)
	- [Crypto](https://dev.stormbyte.org/StormByte-Crypto)
	- **Database**
	- [Logger](https://github.com/StormBytePP/StormByte-Logger.git)
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
- Database optional backends:

  - SQLite3
  - PostgreSQL
  - MariaDB

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

#### SQLite

Below are short examples showing how to open a connection and run simple queries using the bundled optional backends.

##### SQLite example

This example shows an in-process SQLite database (file or in-memory). It uses the StormByte `SQLite` optional backend which exposes the same `Database`-style API as other backends.

```cpp
#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/logger/log.hxx>

auto logger = std::make_shared<StormByte::Logger::Log>(std::cout, StormByte::Logger::Level::Info);

// Open an in-memory SQLite database
StormByte::Database::SQLite db(":memory:", logger);
if (!db.Connect()) {
	// handle connection error
}

// Execute a simple query
auto res = db.Query("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT);");
// use PrepareSTMT / ExecuteSTMT for prepared statements

db.Disconnect();
```

##### PostgreSQL example

This example uses the optional Postgres backend which delegates to libpq. The API mirrors the SQLite backend — constructors and connection semantics differ (host/user/password/db).

```cpp
#include <StormByte/database/postgres/postgres.hxx>
#include <StormByte/logger/log.hxx>

auto logger = std::make_shared<StormByte::Logger::Log>(std::cout, StormByte::Logger::Level::Info);

// Connect to a Postgres server (host, user, password, dbname)
StormByte::Database::Postgres::Postgres pg("localhost", "testuser", "testpass", "stormbyte_test", logger);
if (!pg.Connect()) {
	// handle connection error (inspect logger for details)
}

// Run queries (Query returns ExpectedRows / optional-like)
auto rows = pg.Query("SELECT 1 as one;");
if (rows.has_value()) {
	// inspect rows.value()
}

// Use prepared statements:
pg.PrepareSTMT("get_users", "SELECT name, email FROM users");
auto users = pg.ExecuteSTMT("get_users");

pg.Disconnect();
```

##### MariaDB example

This example uses the optional MariaDB backend which delegates to MariaDB Connector/C (libmariadb). The API mirrors the other backends and uses secure server-side prepared statements internally; public headers do not expose `mysql.h`.

```cpp
#include <StormByte/database/mariadb/mariadb.hxx>
#include <StormByte/logger/log.hxx>

auto logger = std::make_shared<StormByte::Logger::Log>(std::cout, StormByte::Logger::Level::Info);

// Connect to a MariaDB server (host, user, password, dbname)
StormByte::Database::MariaDB::MariaDB db("localhost", "testuser", "testpass", "stormbyte_test", logger);
if (!db.Connect()) {
	// handle connection error (inspect logger for details)
}

// Run queries
auto rows = db.Query("SELECT 1 as one;");
if (rows.has_value()) {
	// inspect rows.value()
}

// Use prepared statements (server-side prepared statements are used when available)
db.PrepareSTMT("get_users", "SELECT name, email FROM users");
auto users = db.ExecuteSTMT("get_users");

db.Disconnect();
```

Build notes: enable the MariaDB optional backend with `-DWITH_MARIADB=BUNDLED` (bundled connector) or `-DWITH_MARIADB=SYSTEM` (requires `libmariadb-dev`/system connector).


## Contributing

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.

## License

This project is licensed under LGPL v3 License - see the [LICENSE](LICENSE) file for details.
