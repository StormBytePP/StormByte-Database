
#include <StormByte/database/exception.hxx>
#include <StormByte/database/rows.hxx>

using namespace StormByte::Database;

// Index access (const)
const Row& Rows::operator[](std::size_t index) const & {
	if (index >= m_rows.size())
		throw OutOfBounds(static_cast<int>(index), m_rows.size());
	return m_rows[index];
}

// Index access (mutable)
Row& Rows::operator[](std::size_t index) & {
	return const_cast<Row&>(static_cast<const Rows*>(this)->operator[](index));
}

// Index access for rvalue Rows: move out the row
Row Rows::operator[](std::size_t index) && {
	if (index >= m_rows.size())
		throw OutOfBounds(static_cast<int>(index), m_rows.size());
	return std::move(m_rows[index]);
}

