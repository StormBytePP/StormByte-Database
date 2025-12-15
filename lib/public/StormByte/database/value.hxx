#pragma once

#include <StormByte/database/exception.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/database/visibility.h>
#include <StormByte/type_traits.hxx>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Value
	 * @brief Value class for databases
	 */
	class STORMBYTE_DATABASE_PUBLIC Value {
		public:
			/**
			 * @enum Type
			 * @brief Type of a value
			 */
			enum class Type: unsigned short {
				Null		= 0,								///< Null type
				Integer,										///< Integer type
				LongInteger,									///< Long Integer type
				Double,											///< Double type
				Text,											///< Text type
				Blob,											///< Blob type
				Boolean											///< Boolean type
			};

			/**
			 * Constructors to different types
			 * @param value Value to store
			 * @{
			 */
			Value() noexcept:
			m_value(std::monostate{}), m_type(Type::Null) {}

			Value(int value) noexcept:
			m_value(value), m_type(Type::Integer) {}

			Value(long int value) noexcept:
			m_value(value), m_type(Type::LongInteger) {}

			Value(double value) noexcept:
			m_value(value), m_type(Type::Double) {}

			Value(const std::string& value) noexcept:
			m_value(value), m_type(Type::Text) {}

			Value(std::string&& value) noexcept:
			m_value(std::move(value)), m_type(Type::Text) {}

			Value(const char* value) noexcept:
			Value(std::string(value)) {}

			Value(const std::vector<std::byte>& value) noexcept:
			m_value(value), m_type(Type::Blob) {}

			Value(std::vector<std::byte>&& value) noexcept:
			m_value(std::move(value)), m_type(Type::Blob) {}

			Value(bool value) noexcept:
			m_value(value), m_type(Type::Boolean) {}
			/** @} */

			/**
			 * Copy Constructor
			 */
			Value(const Value&)									= default;

			/**
			 * Move Constructor
			 */
			Value(Value&&) noexcept								= default;

			/**
			 * Assignment operator
			 */
			Value& operator=(const Value&)						= default;

			/**
			 * Move operator
			 */
			Value& operator=(Value&&) noexcept					= default;

			/**
			 * Destructor
			 */
			~Value() noexcept									= default;

			template<typename T>
			requires StormByte::Type::VariantHasType<ValuesVariant, std::decay_t<T>>
			std::decay_t<T>& 									Get() & {
				if (auto p = std::get_if<std::decay_t<T>>(&m_value)) return *p;
				throw WrongValueType("Requested type does not match stored type.");
			}

			template<typename T>
			requires StormByte::Type::VariantHasType<ValuesVariant, std::decay_t<T>>
			const std::decay_t<T>& 								Get() const& {
				if (auto p = std::get_if<std::decay_t<T>>(&m_value)) return *p;
				throw WrongValueType("Requested type does not match stored type.");
			}

			template<typename T>
			requires StormByte::Type::VariantHasType<ValuesVariant, std::decay_t<T>>
			std::decay_t<T> 									Get() && {
				if (auto p = std::get_if<std::decay_t<T>>(&m_value)) return std::move(*p);
				throw WrongValueType("Requested type does not match stored type.");
			}

			/**
			 * Gets the type of the value
			 * @return Type
			 */
			inline Type 										Type() const noexcept {
				return m_type;
			}

			/**
			 * Checks if the value is null
			 * @return true if the value is null, false otherwise
			 */
			inline bool 										IsNull() const noexcept {
				return m_type == Type::Null;
			}

		private:
			ValuesVariant m_value;								///< Internal value storage
			enum Type m_type;									///< Type of the value
	};
}