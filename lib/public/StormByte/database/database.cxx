#include <StormByte/database/database.hxx>

using namespace StormByte::Database;

bool Database::Connect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect enter" << std::endl;

	DoPreConnect();
	bool result = DoConnect();
	if (result) {
		m_connected = true;
		DoPostConnect();
	}

	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Connect leave (" << (result ? "ok" : "fail") << ")" << std::endl;
	return result;
}

void Database::Disconnect() noexcept {
	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect enter" << std::endl;

	DoPreDisconnect();
	DoDisconnect();
	DoPostDisconnect();
	m_connected = false;

	if (m_logger)
		*m_logger << Logger::Level::LowLevel << "Disconnect leave" << std::endl;
}

void Database::PrepareSTMT(std::string&& name, std::string&& query) noexcept {
	DoPrepareSTMT(std::move(name), std::move(query));
}

void Database::DoPrepareSTMT(std::string&& name, std::string&& query) noexcept {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "Preparing statement '" << name << "': " << query << std::endl;

	std::unique_ptr<PreparedSTMT> prepared = CreatePreparedSTMT(std::move(name), std::move(query));
	if (prepared)
		m_prepared_stmts.emplace(prepared->Name(), std::move(prepared));
}

Transaction Database::BeginTransaction(IsolationLevel level) {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "BeginTransaction" << std::endl;
	DoBeginTransaction(level);
	return Transaction(*this);
}

void Database::CommitTransaction() {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "CommitTransaction" << std::endl;
	DoSilentQuery("COMMIT;");
}

void Database::RollbackTransaction() {
	if (m_logger)
		*m_logger << Logger::Level::Debug << "RollbackTransaction" << std::endl;
	DoSilentQuery("ROLLBACK;");
}
