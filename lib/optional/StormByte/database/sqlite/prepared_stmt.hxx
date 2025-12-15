#pragma once

#include <StormByte/database/prepared_stmt.hxx>

class sqlite3_stmt; // Forward declaration so we don't have to depend on sqlite3 headers

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for SQLite
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final: public StormByte::Database::PreparedSTMT {
		friend class SQLite3;
		public:
			/**
			 * Copy constructor (deleted)
			 * @param other Other PreparedSTMT to copy from
			 */
			PreparedSTMT(const PreparedSTMT& other) 					= delete;

			/**
			 * Move constructor
			 * @param other Other PreparedSTMT to move from
			 */
			PreparedSTMT(PreparedSTMT&& other) noexcept					= default;

			/**
			 * Destructor
			 */
			~PreparedSTMT() noexcept override;

			/**
			 * Copy assignment operator (deleted)
			 * @param other Other PreparedSTMT to copy from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(const PreparedSTMT& other)			= delete;

			/**
			 * Move assignment operator
			 * @param other Other PreparedSTMT to move from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(PreparedSTMT&& other) noexcept		= default;

		private:
			/**
			 * SQLite3 statement
			 */
			sqlite3_stmt* m_stmt;

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
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const std::nullptr_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const int& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const unsigned int& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const int64_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const uint64_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const double& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, bool value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const std::string& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void														Bind(const int& index, const std::vector<std::byte>& value) noexcept;

			/**
			 * Executes the prepared statement
			 * @return Resulting Rows
			 */
			ExpectedRows												DoExecute() override;

			/**
			 * Resets the prepared statement
			 */
			void 														Reset() noexcept override;
	};
}
