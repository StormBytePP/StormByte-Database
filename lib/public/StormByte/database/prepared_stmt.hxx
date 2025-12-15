#pragma once

#include <StormByte/database/rows.hxx>

/**
 * @namespace Database
#pragma once

#include <StormByte/database/rows.hxx>
#include <vector>
#include <cstddef>

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
			 */
			PreparedSTMT(const std::string& name, const std::string& query) noexcept:
			m_name(name), m_query(query) {}

			/**
			 * Constructor moving string
			 * @param name The name of the prepared statement
			 * @param query The query to prepare
			 */
			PreparedSTMT(std::string&& name, std::string&& query) noexcept:
			m_name(std::move(name)), m_query(std::move(query)) {}

			/**
			 * Default copy constructor (deleted)
			 * @param other Other PreparedSTMT to copy from
			 */
			PreparedSTMT(const PreparedSTMT& other)							= delete;

			/**
			 * Default move constructor
			 * @param other Other PreparedSTMT to move from
			 */
			PreparedSTMT(PreparedSTMT&& other)								= default;

			/**
			 * Default destructor.
			 */
			virtual ~PreparedSTMT()											= default;

			/**
			 * Default copy assignment operator (deleted)
			 * @param other Other PreparedSTMT to copy from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(const PreparedSTMT& other)				= delete;

			/**
			 * Default move assignment operator
			 * @param other Other PreparedSTMT to move from
			 * @return Reference to this PreparedSTMT
			 */
			PreparedSTMT& operator=(PreparedSTMT&& other)					= default;

			/**
			 * Executes the prepared statement with the given arguments
			 * @tparam Args Types of the arguments
			 * @param args Arguments to bind and execute
			 * @return Resulting rows
			 */
			template<typename... Args>
			ExpectedRows													Execute(Args&&... args) {
				Reset();
				// Bind implementations expect a zero-based index and add +1 internally
				std::size_t idx = 0;
				(void)( ( Bind(static_cast<int>(idx++), std::forward<Args>(args)) ), ... );
				ExpectedRows result = DoExecute();
				Reset();
				return result;
			}

			/**
			 * Gets the name of the prepared statement
			 * @return name
			 */
			inline const std::string&										Name() const noexcept {
				return m_name;
			}

			/**
			 * Gets the query of the prepared statement
			 * @return query
			 */
			inline const std::string&										Query() const noexcept {
				return m_query;
			}

		protected:
			std::string m_name;												///< Name of the prepared statement
			std::string m_query;											///< Query to prepare

		private:
			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const std::nullptr_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const int& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const unsigned int& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const int64_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const uint64_t& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const double& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, bool value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			virtual void													Bind(const int& index, const std::string& value) noexcept = 0;

			/**
			 * Binds a blob value to a prepared statement
			 */
			virtual void													Bind(const int& index, const std::vector<std::byte>& value) noexcept = 0;

			/**
			 * Binds a value to a prepared statement
			 * @param index parameter index
			 * @param value Value to be bound
			 */
			inline void														Bind(const int& index, const char* value) noexcept {
				Bind(index, std::string(value));
			}

			/**
			 * Resets the prepared statement
			 */
			virtual void													Reset() noexcept = 0;

			/**
			 * Executes the prepared statement
			 * @return Resulting rows
			 */
			virtual ExpectedRows											DoExecute() = 0;
	};
}