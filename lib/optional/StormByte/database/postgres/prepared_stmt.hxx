#pragma once

#include <StormByte/database/prepared_stmt.hxx>
#include <StormByte/database/value.hxx>

#include <vector>
#include <string>
#include <memory>

struct pg_conn;

/**
 * @namespace Postgres
 * @brief PostgreSQL backend for StormByte::Database.
 */
namespace StormByte::Database::Postgres {
	/**
	 * @class PreparedSTMT
	 * @brief PostgreSQL prepared statement (PQexecPrepared).
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT final : public StormByte::Database::PreparedSTMT {
		friend class Postgres;
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
		~PreparedSTMT() noexcept override = default;

		/**
		 * Copy assignment (deleted).
		 */
		PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

		/**
		 * Move assignment.
		 */
		PreparedSTMT& operator=(PreparedSTMT&& other) noexcept = default;

	private:
		struct pg_conn* m_conn;							///< Connection handle
		std::string m_stmt_name;						///< Server-side statement name

		std::vector<const char*> m_param_values;		///< Bind value pointers
		std::vector<int> m_param_lengths;				///< Bind lengths (blobs)
		std::vector<int> m_param_formats;				///< 0 = text, 1 = binary
		std::vector<std::string> m_string_storage;		///< Owns text/numeric string params
		std::vector<std::vector<char>> m_blob_storage;	///< Owns blob params

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
		 * Stores a bound value at @p index.
		 * @param index Parameter index.
		 * @param value Value to bind.
		 */
		void Binder(const int& index, Value&& value) noexcept override;

		/**
		 * Executes via PQexecPrepared.
		 * @return Result rows or an error.
		 */
		ExpectedRows DoExecute() override;

		/**
		 * Clears all bind storage.
		 */
		void Reset() noexcept override;
	};
}
