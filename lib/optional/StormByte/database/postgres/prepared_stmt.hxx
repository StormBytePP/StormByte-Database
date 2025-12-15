#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <vector>
#include <string>
#include <memory>

// forward-declare the libpq connection struct without including libpq-fe.h
struct pg_conn;

namespace StormByte::Database::Postgres {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for PostgreSQL databases
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final: public StormByte::Database::PreparedSTMT {
		friend class Postgres;
		public:
			/**
			 * Default copy constructor (deleted)
			 * @param other Other PreparedSTMT to copy from
			 */
			PreparedSTMT(const PreparedSTMT& other) 						= delete;

			/**
			 * Default move constructor
			 * @param other Other PreparedSTMT to move from
			 */
			PreparedSTMT(PreparedSTMT&& other) noexcept 					= default;

			/**
			 * Default destructor.
			 */
			~PreparedSTMT() noexcept override 								= default;

			/**
			 * Default copy assignment operator (deleted)
			 * @param other Other PreparedSTMT to copy from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(const PreparedSTMT& other) 				= delete;

			/**
			 * Default move assignment operator
			 * @param other Other PreparedSTMT to move from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(PreparedSTMT&& other) noexcept 			= default;

		private:
			struct pg_conn* m_conn;											///< Pointer to the Postgres connection
			std::string m_stmt_name;										///< Prepared statement name on server

			// parameter storage
			std::vector<const char*> m_param_values;						///< Parameter values
			std::vector<int> m_param_lengths;								///< Parameter lengths
			std::vector<int> m_param_formats;								///< Parameter formats (0=text,1=binary)
			std::vector<std::string> m_string_storage;						///< Storage for string parameters
			std::vector<std::vector<char>> m_blob_storage;					///< Storage for blob parameters

			/**
			 * Constructor
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			PreparedSTMT(const std::string& name, const std::string& query);

			/**
			 * Constructor moving strings
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			PreparedSTMT(std::string&& name, std::string&& query) noexcept;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const std::nullptr_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const int& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const unsigned int& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const int64_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const uint64_t& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const double& value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, bool value) noexcept override;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			void 															Bind(const int& index, const std::string& value) noexcept override;

			/**
			 * Binds a blob value to a prepared statement
			 * @param index parameter index
			 * @param value Blob value to be bound
			 */
			void 															Bind(const int& index, const std::vector<std::byte>& value) noexcept override;

			/**
			 * Executes the prepared statement
			 * @return ExpectedRows containing the result rows or an ExecuteError on failure
			 */
			ExpectedRows 													DoExecute() override;

			/**
			 * Resets the prepared statement bindings
			 */
			void 															Reset() noexcept override;
		};
}
