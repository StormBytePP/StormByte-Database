#pragma once

#include <StormByte/database/rows.hxx>
#include <StormByte/database/value.hxx>
#include <StormByte/logger/log.hxx>

#include <memory>
#include <string>
#include <utility>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class PreparedSTMT
	 * @brief Prepared statement for databases
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT {
		public:
			/**
			 * Default constructor
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @param logger Logger instance
			 */
			PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(name), m_query(query), m_logger(std::move(logger)) {}

			/**
			 * Constructor moving string
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 * @param logger Logger instance
			 */
			PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(std::move(name)), m_query(std::move(query)), m_logger(std::move(logger)) {}

			/**
			 * Default copy constructor (deleted)
			 * @param other Other PreparedSTMT to copy from
			 */
			PreparedSTMT(const PreparedSTMT& other) = delete;

			/**
			 * Default move constructor
			 * @param other Other PreparedSTMT to move from
			 */
			PreparedSTMT(PreparedSTMT&& other) = default;

			/**
			 * Default destructor.
			 */
			virtual ~PreparedSTMT() = default;

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
			PreparedSTMT& operator=(PreparedSTMT&& other) = default;

			/**
			 * Executes the prepared statement with the given arguments
			 * @tparam Args Types of the arguments
			 * @param args Arguments to bind and execute
			 * @return Resulting rows
			 */
			template<typename... Args>
			ExpectedRows Execute(Args&&... args) {
				Reset();
				std::size_t idx = 0;
				(void)((Bind(static_cast<int>(idx++), std::forward<Args>(args))), ...);
				ExpectedRows result = DoExecute();
				Reset();
				return result;
			}

			/**
			 * Gets the name of the prepared statement
			 * @return name
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

			/**
			 * Gets the query of the prepared statement
			 * @return query
			 */
			inline const std::string& Query() const noexcept {
				return m_query;
			}

		protected:
			std::string m_name;												///< Name of the prepared statement
			std::string m_query;											///< Query to prepare
			std::shared_ptr<Logger::Log> m_logger;							///< Logger instance

			/**
			 * Binds a value to a prepared statement (template entry point)
			 * @tparam T Type of the value
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			template<typename T>
			void Bind(const int& index, T&& value) noexcept {
				Binder(index, Value(std::forward<T>(value)));
			}

			/**
			 * Specialization for nullptr_t
			 */
			void Bind(const int& index, std::nullptr_t) noexcept {
				Binder(index, Value());
			}

		private:
			/**
			 * Implementation of bind (to be overridden by backends)
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void Binder(const int& index, Value&& value) noexcept = 0;

			/**
			 * Resets the prepared statement
			 */
			virtual void Reset() noexcept = 0;

			/**
			 * Executes the prepared statement
			 * @return Resulting rows
			 */
			virtual ExpectedRows DoExecute() = 0;
	};
}
