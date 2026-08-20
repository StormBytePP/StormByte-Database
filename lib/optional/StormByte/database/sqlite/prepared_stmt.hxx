#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

class sqlite3_stmt;

/**
 * @namespace SQLite
 * @brief SQLite backend for StormByte::Database.
 */
namespace StormByte::Database::SQLite {
	/**
	 * @class PreparedSTMT
	 * @brief SQLite prepared statement.
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class SQLite3;
	public:
		/**
		 * Copy constructor (deleted).
		 */
		PreparedSTMT(const PreparedSTMT& other) = delete;

		/**
		 * Move constructor.
		 */
		PreparedSTMT(PreparedSTMT&& other) noexcept = default;

		/**
		 * Destructor.
		 */
		~PreparedSTMT() noexcept override;

		/**
		 * Copy assignment (deleted).
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * Move assignment.
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		sqlite3_stmt* m_stmt;	///< SQLite statement handle

		/**
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger);

		/**
		 * @param name Statement name.
		 * @param query SQL text.
		 * @param logger Logger instance.
		 */
		PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept;

		/**
		 * Binds a value at @p index (0-based; SQLite uses 1-based internally).
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * Steps the statement and builds Rows.
		 * @return Result rows or an error.
		 */
		ExpectedRows DoExecute() override;

		/**
		 * Clears bindings and resets the statement.
		 */
		void Reset() noexcept override;
	};
}
