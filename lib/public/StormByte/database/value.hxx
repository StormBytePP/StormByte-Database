#pragma once

#include <StormByte/database/exception.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/database/visibility.h>
#include <StormByte/type_traits.hxx>
#include <cmath>
#include <limits>
#include <type_traits>

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
				UnsignedInteger,								///< Unsigned integer type
				LongInteger,									///< Long Integer type
				UnsignedLongInteger,							///< Unsigned long integer type
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

			Value(unsigned int value) noexcept:
			m_value(value), m_type(Type::UnsignedInteger) {}

			Value(long int value) noexcept:
			m_value(value), m_type(Type::LongInteger) {}

			Value(unsigned long int value) noexcept:
			m_value(value), m_type(Type::UnsignedLongInteger) {}

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
			 * Equality operator
			 * @param other Other Value to compare with
			 * @return True if the values are equal, false otherwise
			 */
			inline bool 										operator==(const Value& other) const noexcept {
				return m_value == other.m_value;
			}

			/**
			 * Inequality operator
			 * @param other Other Value to compare with
			 * @return True if the values are not equal, false otherwise
			 */
			inline bool 										operator!=(const Value& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * Destructor
			 */
			virtual ~Value() noexcept							= default;

			template<typename T>
			requires StormByte::Type::VariantHasType<ValuesVariant, std::decay_t<T>>
			std::decay_t<T>										Get() const {
				using To = std::decay_t<T>;
				// visitor handles all stored alternatives and performs safe numeric conversions
				return std::visit([](auto&& val) -> To {
					using From = std::decay_t<decltype(val)>;
					if constexpr (std::is_same_v<From, std::monostate>) {
						throw WrongValueType("Requested type does not match stored type (null).");
					} else if constexpr (std::is_same_v<From, To>) {
						return val;
					} else if constexpr (std::is_arithmetic_v<From> && std::is_arithmetic_v<To>) {
						return convert_numeric<To, From>(val);
					} else {
						throw WrongValueType("Requested type does not match stored type.");
					}
				}, m_value);
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
			// Helper: safe numeric conversion; only instantiated for arithmetic To and From
			template<typename To, typename From>
			requires (std::is_arithmetic_v<To> && std::is_arithmetic_v<From>)
			static To convert_numeric(const From& val) {
				if constexpr (std::is_integral_v<From> && std::is_integral_v<To>) {
					if constexpr (std::is_signed_v<From>) {
						std::intmax_t from = static_cast<std::intmax_t>(val);
						if constexpr (std::is_signed_v<To>) {
							if (from < static_cast<std::intmax_t>(std::numeric_limits<To>::lowest()) || from > static_cast<std::intmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						} else {
							// From signed, To unsigned
							if (from < 0) throw WrongValueType("Negative value cannot be converted to unsigned.");
							if (static_cast<std::uintmax_t>(from) > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						}
					} else {
						// From unsigned
						std::uintmax_t from = static_cast<std::uintmax_t>(val);
						if constexpr (std::is_signed_v<To>) {
							if (from > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						} else {
							// both unsigned
							if (from > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						}
					}
				} else if constexpr (std::is_integral_v<From> && std::is_floating_point_v<To>) {
					return static_cast<To>(val);
				} else if constexpr (std::is_floating_point_v<From> && std::is_integral_v<To>) {
					long double d = static_cast<long double>(val);
					if (!std::isfinite(d)) throw WrongValueType("Non-finite floating conversion to integer.");
					if (std::trunc(d) != d) throw WrongValueType("Floating value has fractional part; would lose data.");
					std::intmax_t tmp = static_cast<std::intmax_t>(d);
					if constexpr (std::is_signed_v<To>) {
						if (tmp < static_cast<std::intmax_t>(std::numeric_limits<To>::lowest()) || tmp > static_cast<std::intmax_t>(std::numeric_limits<To>::max()))
							throw WrongValueType("Floating to integer conversion would overflow/narrow.");
						return static_cast<To>(tmp);
					} else {
						// To unsigned
						if (tmp < 0) throw WrongValueType("Negative value cannot be converted to unsigned.");
						if (static_cast<std::uintmax_t>(tmp) > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
							throw WrongValueType("Floating to integer conversion would overflow/narrow.");
						return static_cast<To>(tmp);
					}
				} else if constexpr (std::is_floating_point_v<From> && std::is_floating_point_v<To>) {
					return static_cast<To>(val);
				} else {
					throw WrongValueType("Unsupported numeric conversion.");
				}
			}
			ValuesVariant m_value;								///< Internal value storage
			enum Type m_type;									///< Type of the value
	};
}