# StormByte-Database

![Linux](https://img.shields.io/badge/Linux-Supported-1793D1?logo=linux&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-Supported-0078D6?logo=windows&logoColor=white)
![macOS](https://img.shields.io/badge/macOS-Supported-0078D6?logo=apple&logoColor=white)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.12+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)
[![CI](https://github.com/StormBytePP/StormByte-Database/actions/workflows/ci.yml/badge.svg)](https://github.com/StormBytePP/StormByte-Database/actions/workflows/ci.yml)

Cross-platform C++26 database abstraction for **SQLite**, **PostgreSQL** and **MariaDB**, with a single API for connections, queries, prepared statements and RAII transactions.

## Features

- Unified API across SQLite, PostgreSQL and MariaDB
- Prepared statements with type-safe binding
- Result rows with index and column-name access
- RAII transactions with configurable isolation levels
- `SslMode` for network backends (Disable / Prefer / Require / Default)
- Logging via [StormByte-Logger](https://github.com/StormBytePP/StormByte-Logger)
- Optional backends (`BUNDLED` / `SYSTEM` / `OFF`) selected at configure time

> **Thread safety:** `Database` instances are **not** thread-safe. Use **one connection per thread**. Concurrent access on the same instance is undefined behaviour; the database engine handles concurrency across separate connections.

## Table of contents

- [Repository](#repository)
- [Installation](#installation)
- [Usage](#usage)
  - [Subclass pattern](#subclass-pattern)
  - [SQLite](#sqlite)
  - [PostgreSQL](#postgresql)
  - [MariaDB](#mariadb)
  - [Transactions](#transactions)
  - [SSL](#ssl)
- [CMake options](#cmake-options)
- [Modules](#modules)
- [Contributing](#contributing)
- [License](#license)

## Repository

[https://github.com/StormBytePP/StormByte-Database](https://github.com/StormBytePP/StormByte-Database)

## Installation

### Prerequisites

- C++26 compiler
- CMake 3.12+
- Optional system libraries if not using bundled backends:
  - SQLite3
  - PostgreSQL (`libpq`)
  - MariaDB Connector/C (`libmariadb`)

### Building

```sh
git clone --recursive https://github.com/StormBytePP/StormByte-Database.git
cd StormByte-Database
cmake -S . -B build \
  -DENABLE_TEST=ON \
  -DWITH_SQLITE=BUNDLED \
  -DWITH_POSTGRES=BUNDLED \
  -DWITH_MARIADB=BUNDLED \
  -DWITH_STORMBYTE=BUNDLED
cmake --build build
ctest --test-dir build/test --output-on-failure
```

## Usage

### Subclass pattern

Backend constructors are **protected**. Typical use is a thin subclass that sets up schema and prepared statements in `DoPostConnect()` (prefer `DoSilentQuery` / `DoPrepareSTMT` inside hooks):

```cpp
#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/logger/log.hxx>
#include <iostream>
#include <memory>

class AppDb : public StormByte::Database::SQLite::SQLite3 {
public:
	AppDb(std::shared_ptr<StormByte::Logger::Log> log)
		: SQLite3(":memory:", log) {}

	auto users() { return ExecuteSTMT("select_users"); }

private:
	void DoPostConnect() noexcept override {
		DoSilentQuery("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT NOT NULL);");
		DoSilentQuery("INSERT INTO users (name) VALUES ('Alice');");
		DoPrepareSTMT("select_users", "SELECT id, name FROM users;");
	}
};

int main() {
	auto logger = std::make_shared<StormByte::Logger::Log>(
		std::cout, StormByte::Logger::Level::Info);

	AppDb db(logger);
	if (!db.Connect())
		return 1;

	auto rows = db.users();
	if (rows.has_value()) {
		for (const auto& row : rows.value()) {
			// row[0], row["name"], ...
		}
	}

	db.Disconnect();
	return 0;
}
```

Public surface after connect: `Query`, `SilentQuery`, `ExecuteSTMT`, `BeginTransaction`, `IsConnected`, `SetSslMode` / `GetSslMode`.

### SQLite

```cpp
#include <StormByte/database/sqlite/sqlite3.hxx>

// In-memory
SQLite3(std::shared_ptr<Logger::Log> logger);

// File
SQLite3(const std::filesystem::path& dbfile, std::shared_ptr<Logger::Log> logger);
```

SQLite ignores `SslMode`. Optional helpers in subclasses: `EnableForeignKeys()`, WAL / `busy_timeout` via `PRAGMA` in `DoPostConnect`.

### PostgreSQL

```cpp
#include <StormByte/database/postgres/postgres.hxx>

Postgres(const std::string& host,
         const std::string& user,
         const std::string& password,
         const std::string& db_name,
         std::shared_ptr<Logger::Log> logger);
```

Placeholders in prepared statements use `$1`, `$2`, …  
Client notices are forwarded to the logger at `Level::Notice`.

### MariaDB

```cpp
#include <StormByte/database/mariadb/mariadb.hxx>

MariaDB(const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& db_name,
        int port,
        std::shared_ptr<Logger::Log> logger);
```

Placeholders use `?`.  
`TEXT` vs binary `BLOB` is distinguished via field charset (`charsetnr == 63` → binary).  
Server warnings are logged at `Level::Notice`.

### Transactions

```cpp
#include <StormByte/database/transaction.hxx>
using StormByte::Database::IsolationLevel;

{
	auto tx = db.BeginTransaction(IsolationLevel::Serializable);
	db.SilentQuery("INSERT INTO users (name) VALUES ('Bob');");
	tx.Commit();   // without Commit/Rollback, destructor rolls back
}
```

| `IsolationLevel`   | Typical mapping                                      |
|--------------------|------------------------------------------------------|
| `Default`          | Backend default                                      |
| `ReadUncommitted`  | Where supported (SQLite → `BEGIN DEFERRED`)          |
| `ReadCommitted`    | PostgreSQL / MariaDB default                         |
| `RepeatableRead`   | Supported on PG/MariaDB; SQLite → `BEGIN IMMEDIATE`  |
| `Serializable`     | Highest isolation; SQLite → `BEGIN EXCLUSIVE`        |

### SSL

Network backends only (PostgreSQL / MariaDB):

```cpp
using StormByte::Database::SslMode;

db.SetSslMode(SslMode::Disable);  // local / CI without TLS
db.SetSslMode(SslMode::Prefer);
db.SetSslMode(SslMode::Require);
db.Connect();
```

| Mode       | PostgreSQL `sslmode` | MariaDB (when available)   |
|------------|----------------------|----------------------------|
| `Default`  | client default       | client default             |
| `Disable`  | `disable`            | `SSL_MODE_DISABLED`        |
| `Prefer`   | `prefer`             | `SSL_MODE_PREFERRED`       |
| `Require`  | `require`            | `SSL_MODE_REQUIRED`        |

## CMake options

| Option            | Values                         | Meaning                          |
|-------------------|--------------------------------|----------------------------------|
| `WITH_SQLITE`     | `BUNDLED` / `SYSTEM` / `OFF`   | SQLite backend                   |
| `WITH_POSTGRES`   | `BUNDLED` / `SYSTEM` / `OFF`   | PostgreSQL backend               |
| `WITH_MARIADB`    | `BUNDLED` / `SYSTEM` / `OFF`   | MariaDB backend                  |
| `WITH_STORMBYTE`  | `BUNDLED` / `SYSTEM`           | Core StormByte dependency        |
| `ENABLE_TEST`     | `ON` / `OFF`                   | Build tests                      |
| `ENABLE_DOC`      | `ON` / `OFF`                   | Doxygen target                   |

## Modules

StormByte is split into several libraries:

- [Base](https://dev.stormbyte.org/StormByte)
- [Buffer](https://dev.stormbyte.org/StormByte-Buffer)
- [Config](https://dev.stormbyte.org/StormByte-Config)
- [Crypto](https://dev.stormbyte.org/StormByte-Crypto)
- **Database** (this repository)
- [Logger](https://github.com/StormBytePP/StormByte-Logger)
- [Multimedia](https://dev.stormbyte.org/StormByte-Multimedia)
- [Network](https://dev.stormbyte.org/StormByte-Network)
- [System](https://dev.stormbyte.org/StormByte-System)

## Contributing

Fork the repository and open a pull request. Please keep the coding style (opening brace on the same line as the declaration, Doxygen on public headers, English comments only when necessary) and run the test suite before submitting.

## License

LGPL v3 — see [LICENSE](LICENSE).
