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
	 * @brief Abstract prepared statement (backend-specific subclasses).
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT {
		public:
			/**
			 * @param name Statement name.
			 * @param query SQL text.
			 * @param logger Logger instance.
			 */
			PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(name), m_query(query), m_logger(std::move(logger)) {}

			/**
			 * @param name Statement name.
			 * @param query SQL text.
			 * @param logger Logger instance.
			 */
			PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(std::move(name)), m_query(std::move(query)), m_logger(std::move(logger)) {}

			/**
			 * Copy constructor (deleted).
			 */
			PreparedSTMT(const PreparedSTMT& other) = delete;

			/**
			 * Move constructor.
			 */
			PreparedSTMT(PreparedSTMT&& other) = default;

			/**
			 * Destructor.
			 */
			virtual ~PreparedSTMT() = default;

			/**
			 * Copy assignment (deleted).
			 */
			PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

			/**
			 * Move assignment.
			 */
			PreparedSTMT& operator=(PreparedSTMT&& other) = default;

			/**
			 * Binds arguments and executes the statement.
			 * @tparam Args Argument types.
			 * @param args Positional bind values (0-based).
			 * @return Result rows or an error.
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
			 * @return Statement name.
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

			/**
			 * @return SQL text.
			 */
			inline const std::string& Query() const noexcept {
				return m_query;
			}

		protected:
			std::string m_name;							///< Statement name
			std::string m_query;						///< SQL text
			std::shared_ptr<Logger::Log> m_logger;		///< Logger instance

			/**
			 * Binds a value at @p index.
			 * @tparam T Value type.
			 * @param index Parameter index (0-based).
			 * @param value Value to bind.
			 */
			template<typename T>
			void Bind(const int& index, T&& value) noexcept {
				Binder(index, Value(std::forward<T>(value)));
			}

			/**
			 * Binds SQL NULL at @p index.
			 * @param index Parameter index (0-based).
			 */
			void Bind(const int& index, std::nullptr_t) noexcept {
				Binder(index, Value());
			}

		private:
			/**
			 * Backend bind implementation.
			 * @param index Parameter index (0-based).
			 * @param value Value to bind.
			 */
			virtual void Binder(const int& index, Value&& value) noexcept = 0;

			/**
			 * Resets bindings / statement state.
			 */
			virtual void Reset() noexcept = 0;

			/**
			 * Executes the prepared statement.
			 * @return Result rows or an error.
			 */
			virtual ExpectedRows DoExecute() = 0;
	};
}
