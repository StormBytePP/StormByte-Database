#pragma once

#include <StormByte/expected.hxx>
#include <StormByte/database/exception.hxx>

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

/**
 * @namespace StormByte::Database
 * @brief Database abstraction layer shared by all backends.
 */
namespace StormByte::Database {
	class Rows;

	/**
	 * @typedef ValuesVariant
	 * @brief Variant holding the supported column value types.
	 *
	 * std::monostate represents SQL NULL.
	 */
	using ValuesVariant = std::variant<
		std::monostate,
		int,
		unsigned int,
		long int,
		unsigned long int,
		double,
		std::string,
		bool,
		std::vector<std::byte>
	>;

	/**
	 * @typedef ExpectedRows
	 * @brief Result of a query: either a Rows object or a QueryException.
	 */
	using ExpectedRows = Expected<Rows, QueryException>;

	/**
	 * @enum SslMode
	 * @brief TLS policy for network backends (MariaDB, PostgreSQL).
	 *
	 * SQLite ignores this setting. Default leaves the client library behaviour unchanged.
	 */
	enum class SslMode {
		Default,	///< Client / driver default
		Disable,	///< Do not use TLS
		Prefer,		///< Prefer TLS when available, allow plaintext fallback if supported
		Require		///< Require TLS; connection fails if TLS cannot be established
	};

	/**
	 * @enum IsolationLevel
	 * @brief Transaction isolation level requested via BeginTransaction().
	 *
	 * Mapping is backend-specific (e.g. SQLite maps several levels to DEFERRED/IMMEDIATE/EXCLUSIVE).
	 */
	enum class IsolationLevel {
		Default,			///< Backend default isolation
		ReadUncommitted,	///< Dirty reads allowed where supported
		ReadCommitted,		///< Only committed data is visible
		RepeatableRead,		///< Stable reads within the transaction
		Serializable		///< Full serializability where supported
	};
}
