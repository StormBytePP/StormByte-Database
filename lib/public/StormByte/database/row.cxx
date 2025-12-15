#include <StormByte/database/exception.hxx>
#include <StormByte/database/row.hxx>

using namespace StormByte::Database;

// Index access (const)
const Value& Row::operator[](std::size_t index) const & {
	if (index >= m_values.size())
		throw OutOfBounds(static_cast<int>(index), m_values.size());
	return m_values[index].second;
}

// Index access (mutable)
Value& Row::operator[](std::size_t index) & {
	return const_cast<Value&>(static_cast<const Row*>(this)->operator[](index));
}

// Index access for rvalue Row: move out the value
Value Row::operator[](std::size_t index) && {
	if (index >= m_values.size())
		throw OutOfBounds(static_cast<int>(index), m_values.size());
	return std::move(m_values[index].second);
}

// Name-based access: search optional name in storage and return the value
const Value& Row::operator[](const std::string& columnName) const & {
	for (const auto& p : m_values) {
		if (p.first == columnName)
			return p.second;
	}
	throw ColumnNotFound(columnName);
}

Value& Row::operator[](const std::string& columnName) & {
	return const_cast<Value&>(static_cast<const Row*>(this)->operator[](columnName));
}

Value Row::operator[](const std::string& columnName) && {
	for (auto& p : m_values) {
		if (p.first == columnName)
			return std::move(p.second);
	}
	throw ColumnNotFound(columnName);
}


