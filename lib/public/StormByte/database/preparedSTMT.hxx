#pragma once

#include <StormByte/database/preparedSTMT.hxx>
#include <StormByte/database/row.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for databases
	 */
	template<class Row> class STORMBYTE_DATABASE_PUBLIC PreparedSTMT {
		public:
			/**
			 * Default constructor
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			constexpr PreparedSTMT(const std::string& name, const std::string& query):m_name(name), m_query(query) {}

			/**
			 * Constructor moving string
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			constexpr PreparedSTMT(std::string&& name, std::string&& query):m_name(name), m_query(std::move(query)) {}

			/**
			 * Default copy constructor (deleted)
			 */
			PreparedSTMT(const PreparedSTMT&)							= delete;

			/**
			 * Default move constructor
			 */
			constexpr PreparedSTMT(PreparedSTMT&&)						= default;

			/**
			 * Default copy assignment operator (deleted)
			 */
			PreparedSTMT& operator=(const PreparedSTMT&)				= delete;

			/**
			 * Default move assignment operator
			 */
			constexpr PreparedSTMT& operator=(PreparedSTMT&&)			= default;

			/**
			 * Default destructor.
			 */
			virtual constexpr ~PreparedSTMT()							= default;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const std::nullptr_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const int& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const unsigned int& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const int64_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const uint64_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const double& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, bool value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual PreparedSTMT&										Bind(const int& index, const std::string& value) noexcept = 0;

			/**
			 * Resets the prepared statement
			 */
			virtual void 												Reset() noexcept = 0;

			/**
			 * Step into the prepared statement results
			 */
			virtual const Row&											Step() noexcept = 0;

			/**
			 * Gets the name of the prepared statement
			 * @return name
			 */
			constexpr const std::string&								GetName() const noexcept {
				return m_name;
			}

			/**
			 * Gets the query of the prepared statement
			 * @return query
			 */
			constexpr const std::string&								GetQuery() const noexcept {
				return m_query;
			}

		protected:
			std::string m_name;					///< Name of the prepared statement
			std::string m_query;				///< Query to prepare
			std::unique_ptr<Row> m_row;			///< Last result row of the prepared statement
	};
}