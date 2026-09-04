# StormByte-Database

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)
![C++26](https://img.shields.io/badge/C%2B%2B-26-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.28+-064F8C?logo=cmake&logoColor=white)
![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)
[![CI](https://github.com/StormBytePP/StormByte-Database/actions/workflows/ci.yml/badge.svg)](https://github.com/StormBytePP/StormByte-Database/actions/workflows/ci.yml)
[![Sponsor](https://img.shields.io/badge/Sponsor-StormBytePP-ea4aaa?logo=githubsponsors)](https://github.com/sponsors/StormBytePP)

This repository is **StormByte Database**: the C++26 SQL layer of the StormByte suite.

One API covers SQLite, PostgreSQL and MariaDB. You do **not** construct those backends as generic objects. They are **base classes**: derive your schema, call the backend constructor, prepare statements and hook connect/disconnect there.

The suite is split on purpose. Base, Buffer, Config, Crypto, Logger, Multimedia, Network and System are **other repositories**.

## What this module does

- **One connection type** — `StormByte::Database::Database` with Connect / Disconnect, Query / SilentQuery, named prepared statements and RAII transactions.
- **Inheritance first** — SQLite3, MariaDB and Postgres constructors are protected. Your application database is a subclass.
- **Values** — type-erased `Value` (NULL, integers, double, text, blob, bool) with safe numeric `Get<T>()`.
- **Rows** — ordered columns, lookup by name (`ColumnNotFound` / `OutOfBounds`).
- **Prepared statements** — bind by position (0-based), `nullptr` is SQL NULL, `ExpectedRows` on execute.
- **Transactions** — `BeginTransaction(IsolationLevel)` returns a `Transaction` that rolls back if you forget Commit.
- **TLS** — `SslMode` for MariaDB and PostgreSQL. SQLite ignores it.
- **Not thread-safe** — one connection per thread.

## The rest of the suite

| Module | Role |
| --- | --- |
| [Base](https://github.com/StormBytePP/StormByte) | Exceptions, Expected, serialization, strings, UUID, concepts |
| [Buffer](https://github.com/StormBytePP/StormByte-Buffer) | FIFO, SharedFIFO, Ring, Producer/Consumer and multi-stage pipelines |
| [Config](https://github.com/StormBytePP/StormByte-Config) | Human-readable text and versioned binary documents (groups, lists, raw bytes) |
| [Crypto](https://github.com/StormBytePP/StormByte-Crypto) | Hash, compress, encrypt, sign and key agreement — Crypto++ never leaves the private tree |
| [Database](https://github.com/StormBytePP/StormByte-Database) | This repository |
| [Logger](https://github.com/StormBytePP/StormByte-Logger) | Stream logger with levels, headers, human-readable sizes and redaction (`ThreadedLog`) |
| [Multimedia](https://github.com/StormBytePP/StormByte-Multimedia) | Decode, encode and containers without raw FFmpeg types; codecs enabled only if present |
| [Network](https://github.com/StormBytePP/StormByte-Network) | Framed packets, Client/Server, IPv4/IPv6 TCP and Buffer pipelines (compress/encrypt) |
| [System](https://github.com/StormBytePP/StormByte-System) | Processes, pipes and environment variables across Linux, Windows and macOS |

Docs sites (when published): [Base](https://dev.stormbyte.org/StormByte), [Buffer](https://dev.stormbyte.org/StormByte-Buffer), [Config](https://dev.stormbyte.org/StormByte-Config), [Crypto](https://dev.stormbyte.org/StormByte-Crypto), [Database](https://dev.stormbyte.org/StormByte-Database), [Logger](https://dev.stormbyte.org/StormByte-Logger), [Multimedia](https://dev.stormbyte.org/StormByte-Multimedia), [Network](https://dev.stormbyte.org/StormByte-Network), [System](https://dev.stormbyte.org/StormByte-System).

## Table of Contents

- [What this module does](#what-this-module-does)
- [The rest of the suite](#the-rest-of-the-suite)
- [Installation](#installation)
- [Usage](#usage)
  - [Derive your database](#derive-your-database)
  - [Values and rows](#values-and-rows)
  - [Queries and statements](#queries-and-statements)
  - [Transactions](#transactions)
- [Contributing](#contributing)
- [License](#license)

## Installation

Needs a C++26 compiler and CMake 3.28 or newer. Enable the backends you want (`WITH_SQLITE`, `WITH_POSTGRES`, `WITH_MARIADB`: `OFF`, `SYSTEM` or `BUNDLED`).

```sh
git clone https://github.com/StormBytePP/StormByte-Database.git
cd StormByte-Database
cmake -S . -B build
cmake --build build
```

## Usage

Headers are `#include <StormByte/database/….hxx>`. Namespace root is `StormByte::Database`.

### Derive your database

```cpp
#include <StormByte/database/sqlite/sqlite3.hxx>
#include <StormByte/logger/log.hxx>

class AppDb : public StormByte::Database::SQLite::SQLite3 {
public:
	AppDb(std::shared_ptr<StormByte::Logger::Log> log)
		: SQLite3(std::filesystem::path{"app.db"}, log) {}

protected:
	void DoPostConnect() noexcept override {
		EnableForeignKeys();
		PrepareSTMT("user_by_id", "SELECT id, name FROM users WHERE id = ?");
	}
};

int main() {
	AppDb db(nullptr);
	if (!db.Connect())
		return 1;

	auto rows = db.Query("SELECT 1 AS n");
	if (!rows)
		return 1;
}
```

MariaDB / Postgres follow the same pattern: subclass, pass host / user / password / database (and port on MariaDB), optionally `SetSslMode` before `Connect()`.

### Values and rows

```cpp
#include <StormByte/database/value.hxx>

using namespace StormByte::Database;

Value n(42);
Value empty;          // SQL NULL
auto i = n.Get<int>();
if (auto row = /* from Query */) {
	const Value& name = (*row)[0]["name"];
}
```

### Queries and statements

```cpp
auto result = db.ExecuteSTMT("user_by_id", 7);
if (!result)
	return 1;

for (const auto& row : *result) {
	auto id = row["id"].Get<int>();
}
```

`nullptr` binds SQL NULL. Missing statement names raise `UnknownSTMT` through `ExpectedRows`.

### Transactions

```cpp
{
	auto tx = db.BeginTransaction(IsolationLevel::Serializable);
	db.SilentQuery("INSERT INTO users(name) VALUES ('ada')");
	tx.Commit();
} // Rollback if Commit was not called
```

## Contributing

Issues only on this repository. Fork and open a pull request against `master`.

## License

GNU Lesser General Public License version 3 or later. See [LICENSE](LICENSE) and <https://www.gnu.org/licenses/lgpl-3.0.html>.
