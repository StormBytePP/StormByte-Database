#include <database/sqlite/exception.hxx>

#include <limits>

using namespace StormByte::Database::SQLite;

Exception::Exception(const std::string& msg):StormByte::Exception(msg) {}

WrongResultType::WrongResultType(const Type& source_type, const Type& asked_type):
Exception("Wrong value type: Requested " + std::string(GetTypeAsString(asked_type))
	+ " whereas contained type is " + GetTypeAsString(source_type)) {}

ConnectionError::ConnectionError(const std::string& reason):
Exception(reason) {}

ConnectionError::ConnectionError(std::string&& reason):
Exception(std::move(reason)) {}

QueryError::QueryError(const std::string& reason):
Exception(reason) {}

QueryError::QueryError(std::string&& reason):
Exception(std::move(reason)) {}
