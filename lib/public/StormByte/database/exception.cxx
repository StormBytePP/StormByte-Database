#include <StormByte/database/exception.hxx>

using namespace StormByte::Database;

Exception::Exception(const std::string& reason): StormByte::Exception(reason) {}

Exception::Exception(std::string&& reason): StormByte::Exception(std::move(reason)) {}

WrongValueType::WrongValueType(const std::string& targettype):
Exception("Value requested is not a " + targettype) {}

ColumnNotFound::ColumnNotFound(const std::string& name):
Exception("Column name " + name + " do not exist") {}

OutOfBounds::OutOfBounds(const size_t& index):
Exception("Index " + std::to_string(index) + " is out of bounds") {}