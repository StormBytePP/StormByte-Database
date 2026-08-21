# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- Port SQLite amalgamation to StormByte-BuildMaster (cached download + static PIC build via `create_cmake_component`)
- Bump bundled SQLite to 3.53.4
- Minor indentation adjustments in configure status messages

## [1.0.0] - 2026-08-20

Initial public release of **StormByte-Database**: a C++23 abstraction over SQLite, PostgreSQL and MariaDB with a shared API for connections, queries, prepared statements and transactions.

### Added

- Unified `Database` API for SQLite, PostgreSQL and MariaDB backends
- Optional backends selected at configure time (`WITH_SQLITE`, `WITH_POSTGRES`, `WITH_MARIADB`: `BUNDLED` / `SYSTEM` / `OFF`)
- Prepared statements with type-safe parameter binding
- Result `Rows` / `Row` with access by column index and by name
- Typed `Value` storage (integers, floating point, text, blob, bool, null) with safe numeric conversions
- RAII `Transaction` via `BeginTransaction()` with automatic rollback if neither commit nor rollback is called
- `IsolationLevel` (`Default`, `ReadUncommitted`, `ReadCommitted`, `RepeatableRead`, `Serializable`) mapped per backend
- `SslMode` (`Default`, `Disable`, `Prefer`, `Require`) on `Database` for PostgreSQL and MariaDB
- Integration with StormByte-Logger (query/connect diagnostics; PostgreSQL notices and MariaDB warnings at notice level)
- Lifecycle hooks (`DoPreConnect`, `DoConnect`, `DoPostConnect`, `DoPreDisconnect`, `DoDisconnect`, `DoPostDisconnect`) for schema setup and cleanup
- Comprehensive tests for all three backends (edge cases, blobs, nulls, transactions, isolation, concurrent multi-connection workloads)
- Cross-platform CI (Linux, macOS, Windows)

### Notes

- `Database` instances are **not thread-safe**. Use one connection per thread; use separate connections for concurrent workloads.
- Backend constructors are protected: subclass the backend and set up schema / prepared statements in `DoPostConnect()` (prefer `DoSilentQuery` / `DoPrepareSTMT` inside hooks).
- SQLite ignores `SslMode`. Network backends apply it on `Connect()`.

[1.0.0]: https://github.com/StormBytePP/StormByte-Database/releases/tag/1.0.0