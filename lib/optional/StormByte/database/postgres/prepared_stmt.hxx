#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

#include <vector>
#include <string>
#include <memory>

struct pg_conn;

/**
 * @namespace Postgres
 * @brief All the classes for handling PostgreSQL databases
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for PostgreSQL databases
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class Postgres;
	public:
		/**
		 * Default copy constructor (deleted)
		 * @param other Other PreparedSTMT to copy from
		 */
		PreparedSTMT(const PreparedSTMT& other) = delete;

		/**
		 * Default move constructor
		 * @param other Other PreparedSTMT to move from
		 */
		PreparedSTMT(PreparedSTMT&& other) noexcept = default;

		/**
		 * Default destructor.
		 */
		~PreparedSTMT() noexcept override = default;

		/**
		 * Default copy assignment operator (deleted)
		 * @param other Other PreparedSTMT to copy from
		 * @return Reference to this PreparedSTMT
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * Default move assignment operator
		 * @param other Other PreparedSTMT to move from
		 * @return Reference to this PreparedSTMT
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		struct pg_conn* m_conn;
		std::string m_stmt_name;

		std::vector<const char*> m_param_values;
		std::vector<int> m_param_lengths;
		std::vector<int> m_param_formats;
		std::vector<std::string> m_string_storage;
		std::vector<std::vector<char>> m_blob_storage;

		/**
		 * Constructor
		 * @param name The name of the prepared statement
		 * @param query The query to prepare
		 * @param logger Logger instance
		 */
		PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger);

		/**
		 * Constructor moving strings
		 * @param name The name of the prepared statement
		 * @param query The query to prepare
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
		 * @return ExpectedRows containing the result rows or an ExecuteError on failure
		 */
		ExpectedRows DoExecute() override;

		/**
		 * Resets the prepared statement bindings
		 */
		void Reset() noexcept override;
	};
}
