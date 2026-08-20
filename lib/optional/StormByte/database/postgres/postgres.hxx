#pragma once

#include <StormByte/database/database.hxx>
#include <StormByte/database/postgres/prepared_stmt.hxx>

#include <memory>
#include <string>

struct pg_conn;

/**
 * @namespace Postgres
 * @brief PostgreSQL backend for StormByte::Database.
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class Postgres
	 * @brief PostgreSQL database backend.
	 *
	 * @note Not thread-safe. One instance per thread.
	 *
	 * @note **Inheritance-oriented.** Constructors are protected. Derive your
	 * own class and call the Postgres constructor from your constructor. Optional
	 * SetSslMode() before Connect(). Not intended for direct generic construction.
	 */
	class STORMBYTE_DATABASE_PUBLIC Postgres : public Database {
		public:
			/**
			 * Copy constructor (deleted).
			 */
			Postgres(const Postgres& db) = delete;

			/**
			 * Move constructor.
			 */
			Postgres(Postgres&& db) noexcept = default;

			/**
			 * Copy assignment (deleted).
			 */
			Postgres& operator=(const Postgres& db) = delete;

			/**
			 * Move assignment.
			 */
			Postgres& operator=(Postgres&& db) noexcept = default;

			/**
			 * Destructor.
			 */
			~Postgres() noexcept override;

			/**
			 * Executes a query and returns rows.
			 * @param query SQL text.
			 * @return Result rows or an error.
			 */
			ExpectedRows Query(const std::string& query) noexcept override;

			/**
			 * Executes a query that does not return rows.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool SilentQuery(const std::string& query) noexcept override;

		protected:
			/**
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param logger Logger instance.
			 */
			Postgres(const std::string& host, const std::string& user, const std::string& password,
					const std::string& db_name, std::shared_ptr<Logger::Log> logger);

			/**
			 * Move-string overload.
			 * @param host Host name or address.
			 * @param user User name.
			 * @param password Password.
			 * @param db_name Database name.
			 * @param logger Logger instance.
			 */
			Postgres(std::string&& host, std::string&& user, std::string&& password,
					std::string&& db_name, std::shared_ptr<Logger::Log> logger);

			/**
			 * Internal silent query.
			 * @param query SQL text.
			 * @return true on success.
			 */
			bool DoSilentQuery(const std::string& query) noexcept override;

		private:
			std::string m_host;			///< Host
			std::string m_user;			///< User
			std::string m_password;		///< Password
			std::string m_dbname;		///< Database name
			struct pg_conn* m_conn;		///< Connection handle

			/**
			 * Connects via PQconnectdb.
			 * @return true on success.
			 */
			bool DoConnect() noexcept override;

			/**
			 * Clears prepared statements.
			 */
			void DoPreDisconnect() noexcept override;

			/**
			 * Closes the connection.
			 */
			void DoDisconnect() noexcept override;

			/**
			 * Creates a PostgreSQL prepared statement (PQprepare).
			 * @param name Statement name.
			 * @param query SQL text.
			 * @return Prepared statement or nullptr.
			 */
			std::unique_ptr<StormByte::Database::PreparedSTMT> CreatePreparedSTMT(std::string&& name, std::string&& query) noexcept override;

			/**
			 * BEGIN with isolation level.
			 * @param level Isolation level.
			 */
			void DoBeginTransaction(IsolationLevel level) override;
	};
}
