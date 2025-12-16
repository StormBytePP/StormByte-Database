#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <vector>
#include <string>
#include <memory>
#include <StormByte/database/value.hxx>

struct st_mysql;
struct st_mysql_stmt;

/**
 * @namespace MariaDB
 * @brief All the classes for handling MariaDB databases
 */
namespace StormByte::Database::MariaDB {
	class MariaDB;															///< Forward declaration of MariaDB class

	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for MariaDB databases
	 */
    class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final: public StormByte::Database::PreparedSTMT {
        friend class ::StormByte::Database::MariaDB::MariaDB;
    public:
		/**
		 * Default copy constructor (deleted)
		 * @param other Other PreparedSTMT to copy from
		 */
        PreparedSTMT(const PreparedSTMT& other) 							= delete;

		/**
		 * Default move constructor
		 * @param other Other PreparedSTMT to move from
		 */
        PreparedSTMT(PreparedSTMT&& other) noexcept 						= default;

		/**
		 * Destructor
		 */
        ~PreparedSTMT() noexcept override;

		/**
		 * Default copy assignment operator (deleted)
		 * @param other Other PreparedSTMT to copy from
		 * @return Reference to this PreparedSTMT
		 */
        PreparedSTMT& operator=(const PreparedSTMT& other) 					= delete;

		/**
		 * Default move assignment operator
		 * @param other Other PreparedSTMT to move from
		 * @return Reference to this PreparedSTMT
		 */
        PreparedSTMT& operator=(PreparedSTMT&& other) noexcept 				= default;

    private:
        struct st_mysql* m_conn;											///< MariaDB connection handle
        struct st_mysql_stmt* m_stmt;										///< MariaDB statement handle
        std::vector<StormByte::Database::Value> m_params;					///< Parameters for the prepared statement

		/**
		 * Constructor
		 * @param name The name of the prepared statement
		 * @param query The query to prepare
		 * @param conn MariaDB connection handle
		 */
        PreparedSTMT(const std::string& name, const std::string& query, struct st_mysql* conn);

		/**
		 * Constructor moving strings
		 * @param name The name of the prepared statement
		 * @param query The query to prepare
		 * @param conn MariaDB connection handle
		 */
        PreparedSTMT(std::string&& name, std::string&& query, struct st_mysql* conn) noexcept;

		/**
		 * Ensures that the parameter vector has enough size
		 * @param params Parameter vector
		 * @param index Index to ensure
		 */
		static void 														EnsureParamSize(std::vector<StormByte::Database::Value>& params, int index) noexcept;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const std::nullptr_t& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const int& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const unsigned int& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const int64_t& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const uint64_t& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const double& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, bool value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const std::string& value) noexcept override;

		/**
		 * Binds a value to a prepared statement
		 * @param index parameter index
		 * @param value Value to be bound
		 */
        void 																Bind(const int& index, const std::vector<std::byte>& value) noexcept override;

		/**
		 * Executes the prepared statement
		 * @return Resulting rows
		 */
        StormByte::Database::ExpectedRows 									DoExecute() override;

		/**
		 * Resets the prepared statement
		 */
        void 																Reset() noexcept override;
    };
}
