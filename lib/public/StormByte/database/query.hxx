#pragma once

#include <StormByte/database/row.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Query
	 * @brief Represents a query to be executed in a database.
	 */
	template<class Row> class STORMBYTE_DATABASE_PUBLIC Query {
		public:
			/**
			 * Default constructor
			 * @param query The query to prepare
			 */
			constexpr Query(const std::string& query): m_query(query) {}

			/**
			 *Constructor moving string
			 * @param query The query to prepare
			 */
			constexpr Query(std::string&& query): m_query(std::move(query)) {}

			/**
			 * Default copy constructor (deleted)
			 */
			Query(const Query&)								= delete;

			/**
			 * Default move constructor
			 */
			constexpr Query(Query&&)						= default;

			/**
			 * Default copy assignment operator (deleted)
			 */
			Query& operator=(const Query&)					= delete;

			/**
			 * Default move assignment operator
			 */
			constexpr Query& operator=(Query&&)				= default;

			/**
			 * Default destructor.
			 */
			virtual constexpr ~Query()						= default;

			/**
			 * Step into the query results
			 */
			virtual const Row&								Step() noexcept = 0;

			/**
			 * Gets the query string
			 * @return query
			 */
			constexpr const std::string&					GetQuery() const noexcept {
				return m_query;
			}

		protected:
			std::string m_query;		///< Query to prepare
			std::shared_ptr<Row> m_row;	///< Current row
	};
}