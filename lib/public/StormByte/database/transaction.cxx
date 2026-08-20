#include <StormByte/database/transaction.hxx>
#include <StormByte/database/database.hxx>

using namespace StormByte::Database;

Transaction::Transaction(Database& db) noexcept
	: m_db(&db), m_active(true) {}

Transaction::Transaction(Transaction&& other) noexcept
	: m_db(other.m_db), m_active(other.m_active) {
	other.m_db = nullptr;
	other.m_active = false;
}

Transaction& Transaction::operator=(Transaction&& other) noexcept {
	if (this != &other) {
		if (m_active && m_db)
			m_db->RollbackTransaction();
		m_db = other.m_db;
		m_active = other.m_active;
		other.m_db = nullptr;
		other.m_active = false;
	}
	return *this;
}

Transaction::~Transaction() noexcept {
	if (m_active && m_db)
		m_db->RollbackTransaction();
}

void Transaction::Commit() {
	if (!m_active || !m_db)
		return;
	m_db->CommitTransaction();
	m_active = false;
}

void Transaction::Rollback() {
	if (!m_active || !m_db)
		return;
	m_db->RollbackTransaction();
	m_active = false;
}
