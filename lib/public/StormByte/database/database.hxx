#pragma once

#include <StormByte/database/visibility.h>

#include <map>
#include <memory>
#include <string>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Database
	 * @brief Abstract database class for database handling
	 */
	template<class Query, class PreparedSTMT> class STORMBYTE_DATABASE_PUBLIC Database {
		public:
			/**
			 * Default constructor.
			 */
			Database()											= default;

			/**
			 * Default copy constructor (deleted)
			 */
			Database(const Database&)							= delete;

			/**
			 * Default move constructor
			 */
			Database(Database&&)								= default;

			/**
			 * Default copy assignment operator (deleted)
			 */
			Database& operator=(const Database&)				= delete;

			/**
			 * Default move assignment operator
			 */
			Database& operator=(Database&&)						= default;

			/**
			 * Default destructor.
			 */
			virtual ~Database()									= default;

			/**
			 * Connects to the database.
			 */
			virtual void 										Connect()		= 0;

			/**
			 * Disconnects from the database.
			 */
			virtual void 										Disconnect()	= 0;

			/**
			 * Executes a query
			 * @param query The query to execute.
			 * @return The created query
			 */
			std::unique_ptr<Query>								PrepareQuery(const std::string& query) {
				return InternalQuery(query);
			}

			/**
			 * Executes a query without returning any result
			 * @param query The query to execute.
			 */
			void 												SilentQuery(const std::string& query) {
				InternalQuery(query)->Step();
			}

			/**
			 * Prepares a statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			void 												PrepareSTMT(const std::string& name, const std::string& query) {
				m_prepared_stmts.insert({name, InternalPrepare(name, query)});
			}

			/**
			 * Prepares all the statements
			 * @param queries The queries to prepare
			 */
			void 												PrepareAll(const std::map<std::string, std::string>& queries) {
				for (const auto& [name, query]: queries) {
					PrepareSTMT(name, query);
				}
			}

			/**
			 * Gets a prepared statement
			 * @param name The name of the prepared statement
			 * @return The prepared statement
			 */
			PreparedSTMT& 										GetPreparedSTMT(const std::string& name) const {
				return *m_prepared_stmts.at(name);
			}

			/**
			 * Begins a transaction
			 */
			virtual void 										BeginTransaction() {
				SilentQuery("BEGIN TRANSACTION;");
			}

			/**
			 * Begins an exclusive transaction
			 */
			virtual void 										BeginExclusiveTransaction() {
				SilentQuery("BEGIN EXCLUSIVE TRANSACTION;");
			}

			/**
			 * Commits the transaction
			 */
			virtual void 										CommitTransaction() {
				SilentQuery("COMMIT;");
			}

			/**
			 * Rolls back the transaction
			 */
			virtual void 										RollbackTransaction() {
				SilentQuery("ROLLBACK;");
			}

			/**
			 * Gets the last error
			 * @return The last error
			 */
			virtual const std::string							LastError() const = 0;

		protected:
			std::map<std::string, std::unique_ptr<PreparedSTMT>> m_prepared_stmts;	///< Prepared statements

			/**
			 * Prepares a statement
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @return The created prepared statement
			 */
			virtual std::unique_ptr<PreparedSTMT>				InternalPrepare(const std::string& name, const std::string& query) = 0;

			/**
			 * Executes a query
			 * @param query The query to execute.
			 * @return The created query
			 */
			virtual std::unique_ptr<Query>						InternalQuery(const std::string& query) = 0;
	};
}