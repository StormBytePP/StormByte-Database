#include <StormByte/database/exception.hxx>
#include <StormByte/database/row.hxx>

using namespace StormByte::Database;

// Name-based access: search optional name in storage and return the value
const Value& Row::operator[](const std::string& columnName) const & {
	for (const auto& p : m_data) {
		if (p.Name() == columnName)
			return p;
	}
	throw ColumnNotFound(columnName);
}

Value& Row::operator[](const std::string& columnName) & {
	return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnName));
}

Value Row::operator[](const std::string& columnName) && {
	for (auto& p : m_data) {
		if (p.Name() == columnName)
			return std::move(p);
	}
	throw ColumnNotFound(columnName);
}


