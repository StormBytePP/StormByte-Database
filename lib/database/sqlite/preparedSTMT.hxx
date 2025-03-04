#pragma once

#include <database/preparedSTMT.hxx>
#include <database/sqlite/alias.hxx>

class sqlite3_stmt; // Forward declaration so we don't have to depend on sqlite3 headers
/**
 * @namespace Database::SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for SQLite
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final: public StormByte::Database::PreparedSTMT<Row> {
		friend class SQLite3;
		public:
			/**
			 * Copy constructor
			 */
			PreparedSTMT(const PreparedSTMT&) 							= delete;

			/**
			 * Move constructor
			 */
			PreparedSTMT(PreparedSTMT&&) noexcept						= default;

			/**
			 * Assignment operator
			 */
			PreparedSTMT& operator=(const PreparedSTMT&)				= delete;

			/**
			 * Move operator
			 */
			PreparedSTMT& operator=(PreparedSTMT&&) noexcept			= default;

			/**
			 * Destructor
			 */
			~PreparedSTMT() noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, const std::nullptr_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, const int& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, const int64_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, const double& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, bool value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			PreparedSTMT&												Bind(const int& index, const std::string& value) noexcept override;


			/**
			 * Resets the prepared statement
			 */
			void 														Reset() noexcept override;

			/**
			 * Executes the prepared statement
			 * @return row pointer (nullptr if there are no more rows)
			 */
			const Row&											Step() noexcept override;

		private:
			/**
			 * Constructor
			 * @param name name
			 * @param query query
			 */
			PreparedSTMT(const std::string& name, const std::string& query);

			/**
			 * Constructor
			 * @param name name
			 * @param query query
			 */
			PreparedSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * SQLite3 statement
			 */
			sqlite3_stmt* m_stmt;
	};
}
