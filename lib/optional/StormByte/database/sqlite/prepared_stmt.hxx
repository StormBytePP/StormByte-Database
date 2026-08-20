#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

class sqlite3_stmt;

/**
 * @namespace SQLite
 * @brief All the classes for handling SQLite databases
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for SQLite
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class SQLite3;
	public:
		/**
		 * Copy constructor (deleted)
		 * @param other Other PreparedSTMT to copy from
		 */
		PreparedSTMT(const PreparedSTMT& other) = delete;

		/**
		 * Move constructor
		 * @param other Other PreparedSTMT to move from
		 */
		PreparedSTMT(PreparedSTMT&& other) noexcept = default;

		/**
		 * Destructor
		 */
		~PreparedSTMT() noexcept override;

		/**
		 * Copy assignment operator (deleted)
		 * @param other Other PreparedSTMT to copy from
		 * @return Reference to this PreparedSTMT
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * Move assignment operator
		 * @param other Other PreparedSTMT to move from
		 * @return Reference to this PreparedSTMT
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		sqlite3_stmt* m_stmt;

		/**
		 * Constructor
		 * @param name name
		 * @param query query
		 * @param logger Logger instance
		 */
		PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger);

		/**
		 * Constructor
		 * @param name name
		 * @param query query
		 * @param logger Logger instance
		 */
		PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * Executes the prepared statement
		 * @return Resulting Rows
		 */
		ExpectedRows DoExecute() override;

		/**
		 * Resets the prepared statement
		 */
		void Reset() noexcept override;
	};
}
