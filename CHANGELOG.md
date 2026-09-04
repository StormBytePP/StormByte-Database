# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Database is the C++26 SQL layer of the StormByte suite.

One API covers SQLite, PostgreSQL and MariaDB.
Backends are base classes: you derive your schema, prepare statements and hook connect there.
This repository is not Base, Buffer, Config, Crypto, Logger, Multimedia, Network or System.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormBytePP/StormByte-Database/blob/master/README.md)
- License: GNU Lesser General Public License version 3 or later, [LICENSE](https://github.com/StormBytePP/StormByte-Database/blob/master/LICENSE)

## [1.0.0] - 2026-09-05

Initial public release of StormByte Database.

### Added

- **Database** abstract connection: Connect / Disconnect, Query / SilentQuery, named prepared statements, isolation and RAII transactions
- **Inheritance-oriented backends** — SQLite3, MariaDB and Postgres with protected constructors
- **Value** — type-erased SQL cell (NULL, integers, double, text, blob, bool) with safe `Get<T>()`
- **NamedValue**, **Row**, **Rows** — column lookup by name or index
- **PreparedSTMT** — positional binds (0-based), `nullptr` is SQL NULL, `ExpectedRows` on execute
- **Transaction** — rolls back if neither Commit nor Rollback ran
- **SslMode** for MariaDB and PostgreSQL (SQLite ignores it)
- **IsolationLevel** mapped per backend
- Optional backends: `WITH_SQLITE` / `WITH_POSTGRES` / `WITH_MARIADB` as OFF, SYSTEM or BUNDLED
- Exception types: ConnectionError, WrongValueType, ColumnNotFound, OutOfBounds, QueryException, UnknownSTMT, ExecuteError

### Notes

- First stable release of StormByte Database.
- Not thread-safe: one connection per thread.
- Needs a C++26 compiler and CMake ≥ 3.28.

[1.0.0]: https://github.com/StormBytePP/StormByte-Database/releases/tag/1.0.0
