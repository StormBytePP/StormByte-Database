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

#include <StormByte/database/exception.hxx>
#include <StormByte/database/typedefs.hxx>
#include <StormByte/database/visibility.h>
#include <StormByte/type_traits.hxx>

#include <cmath>
#include <limits>
#include <type_traits>

/**
 * @brief Database module of the StormByte suite.
 */
namespace StormByte::Database {
	/**
	 * @class Value
	 * @brief Type-erased SQL value (NULL, integers, double, text, blob, bool).
	 */
	class STORMBYTE_DATABASE_PUBLIC Value {
		public:
			/**
			 * @enum Type
			 * @brief Discriminator for the stored alternative.
			 */
			enum class Type: unsigned short {
				Null = 0,				///< SQL NULL
				Integer,				///< int
				UnsignedInteger,		///< unsigned int
				LongInteger,			///< long int
				UnsignedLongInteger,	///< unsigned long int
				Double,					///< double
				Text,					///< std::string
				Blob,					///< std::vector<std::byte>
				Boolean					///< bool
			};

			/**
			 * @name Construction
			 * @{
			 */
			/**
			 * @brief Default constructor. Stores SQL NULL.
			 */
			Value() noexcept:
			m_value(std::monostate{}), m_type(Type::Null) {}

			/**
			 * @brief From a signed int.
			 * @param value Stored value.
			 */
			Value(int value) noexcept:
			m_value(value), m_type(Type::Integer) {}

			/**
			 * @brief From an unsigned int.
			 * @param value Stored value.
			 */
			Value(unsigned int value) noexcept:
			m_value(value), m_type(Type::UnsignedInteger) {}

			/**
			 * @brief From a signed long int.
			 * @param value Stored value.
			 */
			Value(long int value) noexcept:
			m_value(value), m_type(Type::LongInteger) {}

			/**
			 * @brief From an unsigned long int.
			 * @param value Stored value.
			 */
			Value(unsigned long int value) noexcept:
			m_value(value), m_type(Type::UnsignedLongInteger) {}

			/**
			 * @brief From a double.
			 * @param value Stored value.
			 */
			Value(double value) noexcept:
			m_value(value), m_type(Type::Double) {}

			/**
			 * @brief From a string (copy).
			 * @param value Stored text.
			 */
			Value(const std::string& value) noexcept:
			m_value(value), m_type(Type::Text) {}

			/**
			 * @brief From a string (move).
			 * @param value Stored text.
			 */
			Value(std::string&& value) noexcept:
			m_value(std::move(value)), m_type(Type::Text) {}

			/**
			 * @brief From a C string.
			 * @param value Stored text.
			 */
			Value(const char* value) noexcept:
			Value(std::string(value)) {}

			/**
			 * @brief From a blob (copy).
			 * @param value Stored bytes.
			 */
			Value(const std::vector<std::byte>& value) noexcept:
			m_value(value), m_type(Type::Blob) {}

			/**
			 * @brief From a blob (move).
			 * @param value Stored bytes.
			 */
			Value(std::vector<std::byte>&& value) noexcept:
			m_value(std::move(value)), m_type(Type::Blob) {}

			/**
			 * @brief From a bool.
			 * @param value Stored value.
			 */
			Value(bool value) noexcept:
			m_value(value), m_type(Type::Boolean) {}
			/** @} */

			/**
			 * @brief Copy constructor.
			 */
			Value(const Value&) = default;

			/**
			 * @brief Move constructor.
			 */
			Value(Value&&) noexcept = default;

			/**
			 * @brief Copy assignment.
			 */
			Value& operator=(const Value&) = default;

			/**
			 * @brief Move assignment.
			 */
			Value& operator=(Value&&) noexcept = default;

			/**
			 * @brief Equality of the stored alternatives.
			 * @param other Other value.
			 * @return true if equal.
			 */
			inline bool operator==(const Value& other) const noexcept {
				return m_value == other.m_value;
			}

			/**
			 * @brief Inequality.
			 * @param other Other value.
			 * @return true if not equal.
			 */
			inline bool operator!=(const Value& other) const noexcept {
				return !(*this == other);
			}

			/**
			 * @brief Destructor.
			 */
			virtual ~Value() noexcept = default;

			/**
			 * @brief Stored value as @p T, with safe numeric conversions.
			 * @tparam T Requested type (must be a ValuesVariant alternative).
			 * @return Converted value.
			 * @throws WrongValueType on mismatch or unsafe conversion.
			 */
			template<typename T>
			requires StormByte::Type::VariantHasType<ValuesVariant, std::decay_t<T>>
			std::decay_t<T> Get() const {
				using To = std::decay_t<T>;
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
			 * @brief Discriminator of the stored alternative.
			 * @return Type.
			 */
			inline Type Type() const noexcept {
				return m_type;
			}

			/**
			 * @brief Whether the value is SQL NULL.
			 * @return true if NULL.
			 */
			inline bool IsNull() const noexcept {
				return m_type == Type::Null;
			}

		private:
			/**
			 * @brief Safe numeric conversion between arithmetic types.
			 * @tparam To Destination type.
			 * @tparam From Source type.
			 * @param val Source value.
			 * @return Converted value.
			 * @throws WrongValueType on overflow, sign loss or a non-integral float.
			 */
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
							if (from < 0) throw WrongValueType("Negative value cannot be converted to unsigned.");
							if (static_cast<std::uintmax_t>(from) > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						}
					} else {
						std::uintmax_t from = static_cast<std::uintmax_t>(val);
						if constexpr (std::is_signed_v<To>) {
							if (from > static_cast<std::uintmax_t>(std::numeric_limits<To>::max()))
								throw WrongValueType("Integer conversion would overflow/narrow.");
							return static_cast<To>(from);
						} else {
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

			ValuesVariant m_value;	///< Internal storage
			enum Type m_type;		///< Discriminator
	};
}
