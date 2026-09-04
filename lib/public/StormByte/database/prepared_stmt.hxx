/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte-Database.
*
* StormByte-Database is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* or later, as published by the Free Software Foundation.
*
* StormByte-Database is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte-Database. If not, see
* <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#pragma once

#include <StormByte/database/rows.hxx>
#include <StormByte/database/value.hxx>
#include <StormByte/logger/log.hxx>

#include <memory>
#include <string>
#include <utility>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class PreparedSTMT
	 * @brief Abstract prepared statement. Backends subclass this.
	 */
	class STORMBYTE_DATABASE_PUBLIC PreparedSTMT {
		public:
			/**
			 * @brief Copy name, query and logger.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @param logger Logger instance.
			 */
			PreparedSTMT(const std::string& name, const std::string& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(name), m_query(query), m_logger(std::move(logger)) {}

			/**
			 * @brief Move name, query and logger.
			 * @param name Statement name.
			 * @param query SQL text.
			 * @param logger Logger instance.
			 */
			PreparedSTMT(std::string&& name, std::string&& query, std::shared_ptr<Logger::Log> logger) noexcept
				: m_name(std::move(name)), m_query(std::move(query)), m_logger(std::move(logger)) {}

			/**
			 * @brief Copy constructor (deleted).
			 */
			PreparedSTMT(const PreparedSTMT& other) = delete;

			/**
			 * @brief Move constructor.
			 */
			PreparedSTMT(PreparedSTMT&& other) = default;

			/**
			 * @brief Destructor.
			 */
			virtual ~PreparedSTMT() = default;

			/**
			 * @brief Copy assignment (deleted).
			 */
			PreparedSTMT& operator=(const PreparedSTMT& other) = delete;

			/**
			 * @brief Move assignment.
			 */
			PreparedSTMT& operator=(PreparedSTMT&& other) = default;

			/**
			 * @brief Bind arguments and execute.
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
			 * @brief Statement name.
			 * @return Name.
			 */
			inline const std::string& Name() const noexcept {
				return m_name;
			}

			/**
			 * @brief SQL text.
			 * @return Query.
			 */
			inline const std::string& Query() const noexcept {
				return m_query;
			}

		protected:
			std::string m_name;							///< Statement name
			std::string m_query;						///< SQL text
			std::shared_ptr<Logger::Log> m_logger;		///< Logger instance

			/**
			 * @brief Bind a value at @p index.
			 * @tparam T Value type.
			 * @param index Parameter index (0-based).
			 * @param value Value to bind.
			 */
			template<typename T>
			void Bind(const int& index, T&& value) noexcept {
				Binder(index, Value(std::forward<T>(value)));
			}

			/**
			 * @brief Bind SQL NULL at @p index.
			 * @param index Parameter index (0-based).
			 */
			void Bind(const int& index, std::nullptr_t) noexcept {
				Binder(index, Value());
			}

		private:
			/**
			 * @brief Backend bind.
			 * @param index Parameter index (0-based).
			 * @param value Value to bind.
			 */
			virtual void Binder(const int& index, Value&& value) noexcept = 0;

			/**
			 * @brief Reset bindings / statement state.
			 */
			virtual void Reset() noexcept = 0;

			/**
			 * @brief Execute the prepared statement.
			 * @return Result rows or an error.
			 */
			virtual ExpectedRows DoExecute() = 0;
	};
}
