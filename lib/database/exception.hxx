#pragma once

#include <exception.hxx>
#include <database/visibility.h>

/**
 * @namespace Database
 * @brief Contains classes and functions for database operations.
 */
namespace StormByte::Database {
	/**
	 * @class Exception
	 * @brief Base class for Database exceptions
	 */
	class STORMBYTE_DATABASE_PUBLIC Exception: public StormByte::Exception {
		public:
			/**
			 * Constructor
			 * @param reason
			 */
			Exception(const std::string& reason);

			/**
			 * Constructor
			 * @param reason
			 */
			Exception(std::string&& reason);

			/**
			 * Copy constructor
			 */
			Exception(const Exception&)					= default;

			/**
			 * Move constructor
			 */
			Exception(Exception&&) noexcept				= default;

			/**
			 * Assignment operator
			 */
			Exception& operator=(const Exception&)		= default;

			/**
			 * Move assignment operator
			 */
			Exception& operator=(Exception&&) noexcept	= default;

			/**
			 * Destructor
			 */
			virtual ~Exception() noexcept				= default;
	};

	/**
	 * @class WrongValueType
	 * @brief Exception thrown when value type requested is not correct
	 */
	class STORMBYTE_DATABASE_PUBLIC WrongValueType final: public Exception {
		public:
			/**
			 * Constructor
			 * @param targettype item type trying to be converted
			 */
			WrongValueType(const std::string& targettype);

			/**
			 * Copy constructor
			 */
			WrongValueType(const WrongValueType&)				= default;

			/**
			 * Move constructor
			 */
			WrongValueType(WrongValueType&&)					= default;

			/**
			 * Assignment operator
			 */
			WrongValueType& operator=(const WrongValueType&)	= default;

			/**
			 * Move assignment operator
			 */
			WrongValueType& operator=(WrongValueType&&)			= default;

			/**
			 * Destructor
			 */
			~WrongValueType() noexcept override					= default;
	};

	/**
	 * @class ColumnNotFound
	 * @brief Exception when accessing a not found column
	 */
	class STORMBYTE_DATABASE_PUBLIC ColumnNotFound: public Exception {
		public:
			/**
			 * Constructor
			 * @param name column name
			 */
			ColumnNotFound(const std::string& name);

			/**
			 * Copy constructor
			 */
			ColumnNotFound(const ColumnNotFound&)					= default;

			/**
			 * Move constructor
			 */
			ColumnNotFound(ColumnNotFound&&) noexcept				= default;

			/**
			 * Assignment operator
			 */
			ColumnNotFound& operator=(const ColumnNotFound&)		= default;

			/**
			 * Move operator
			 */
			ColumnNotFound& operator=(ColumnNotFound&&) noexcept 	= default;

			/**
			 * Destructor
			 */
			~ColumnNotFound() noexcept override						= default;
	};

	/**
	 * @class OutOfBounds
	 * @brief Exception when accessing an out of bounds column
	 */
	class STORMBYTE_DATABASE_PUBLIC OutOfBounds: public Exception {
		public:
			/**
			 * Constructor
			 * @param index index
			 */
			OutOfBounds(const size_t& index);

			/**
			 * Copy constructor
			 */
			OutOfBounds(const OutOfBounds&)							= default;

			/**
			 * Move constructor
			 */
			OutOfBounds(OutOfBounds&&) noexcept						= default;

			/**
			 * Assignment operator
			 */
			OutOfBounds& operator=(const OutOfBounds&)				= default;

			/**
			 * Move operator
			 */
			OutOfBounds& operator=(OutOfBounds&&) noexcept 			= default;

			/**
			 * Destructor
			 */
			~OutOfBounds() noexcept override						= default;
	};
}