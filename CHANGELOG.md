# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Summary]

StormByte Database is the C++26 SQL layer of the StormByte suite.

One API covers SQLite, PostgreSQL and MariaDB.
Backends are base classes: you derive your schema, prepare statements and hook connect there.
This repository is not Base, Buffer, Config, Crypto, Logger, Multimedia, Network, String or System.
It requires StormByte-Logger 2.0.0 or newer, which vendors StormByte-String and StormByte Base.
The original StormByte-Database source is dual-licensed under LGPL v3.0-or-later or a commercial license.

If you landed here from a release link and have not read the tree:

- What this module is, how to build it, and short examples: [README.md](https://github.com/StormBytePP/StormByte-Database/blob/master/README.md)
- License: dual LGPL v3.0-or-later / commercial, [LICENSE](https://github.com/StormBytePP/StormByte-Database/blob/master/LICENSE) and [COPYING.LGPLv3](https://github.com/StormBytePP/StormByte-Database/blob/master/COPYING.LGPLv3)

## [Unreleased]

### Changed

- Serialized concurrent operations on each built-in database connection. RAII transactions retain exclusive connection access until commit or rollback and must remain on their creating thread.
- **Breaking**: `BeginTransaction` now returns `Expected<Transaction, TransactionError>` instead of throwing when transaction start fails. `Database` construction may also report allocation failure rather than terminating from a `noexcept` constructor, and public class layouts changed; rebuild consumers against this API revision.
- Ported the main library to BuildMaster 2 HOST with shared/static selection. Static consumers receive flattened private vendor dependencies; vendor archives do not need repacking.
- **Breaking**: The public API contract changed beyond the DLL boundary fix. Inputs and storage were migrated to StormByte 2.0 types (`StormByte::String::String`, `StormByte::BinaryData`, `StormByte::Size`, `StormByte::ByteSize`, `std::string_view`), backend logger ownership is now shared (`StormByte::Shared<Logger::Log>`) instead of raw pointers, and statement/query factory signatures now accept view-based names and SQL text rather than rvalue strings. The exported layout of `Database`, `PreparedSTMT`, `Row`, `Rows`, `Value`, `NamedValue` and backend result containers was also tightened to enforce DLL-safe ownership and out-of-line heap operations; consumers must recompile against this port and update any code that relied on old string, logger, or STL-owning ABI assumptions.
- Updated the public API to StormByte 2.0 types: `StormByte::String::String`, `StormByte::BinaryData`, `StormByte::Size`, `StormByte::ByteSize` and `std::string_view` inputs. Heap-owning DLL-boundary operations are defined out-of-line.
- Updated tests, README and Doxygen configuration for StormByte Base/String/Logger 2.0 and the dual-license terms.
- Expanded automated coverage for numeric boundaries, Row/Rows value semantics, backend scalar and large-binary round-trips, transaction rollback, prepared-statement failures and same-connection concurrency.

### Fixed

- Reject floating-point to integer conversions outside the destination range before casting, including values that round to the unsigned or signed upper bound.
- Preserve zero-length SQLite and MariaDB BLOB bindings as empty BLOB values rather than SQL NULL; reject excess MariaDB prepared-statement parameters consistently.
- Preserve SQLite long-integer bind errors so failed parameter binding is reported by statement execution.

[Unreleased]: https://github.com/StormBytePP/StormByte-Database/compare/1.1.0...HEAD

## [1.1.0] - 2026-09-13

### Fixed

- **Backend connection and transaction handling**
	- Fixed system connector discovery with `WITH_SQLITE=SYSTEM`, `WITH_POSTGRES=SYSTEM` and `WITH_MARIADB=SYSTEM`.
	- Fixed ownership transfer when moving connected SQLite, PostgreSQL and MariaDB backends.
	- Fixed transactions silently continuing after failed `BEGIN` or `COMMIT` commands.
	- Fixed PostgreSQL connection handling for credentials containing quotes or backslashes.
- **Prepared statements and result handling**
	- Fixed PostgreSQL and MariaDB numeric result parsing so invalid values return query errors instead of silent zero values.
	- Fixed PostgreSQL prepared statements with multiple text or numeric parameters.
	- Fixed SQLite prepared statements truncating unsigned integer values.
	- Fixed MariaDB prepared statements interpreting unsigned integers as negative values.
- Fixed `Row` index access to throw the Database `OutOfBounds` exception.

### Changed

- Updated the minimum requirements to StormByte Base 1.1.0 and StormByte-Logger 1.1.0, including component-aware Database exceptions.
- Adopted StormByte Base type concepts for Database value conversions.

[1.1.0]: https://github.com/StormBytePP/StormByte-Database/compare/1.0.0...1.1.0

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
