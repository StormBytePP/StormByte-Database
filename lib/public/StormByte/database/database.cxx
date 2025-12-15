#include <StormByte/database/database.hxx>

using namespace StormByte::Database;

bool Database::Connect() noexcept {
	DoPreConnect();
	bool result = DoConnect();
	if (result) {
		m_connected = true;
		DoPostConnect();
	}
	return result;
}

void Database::Disconnect() noexcept {
	DoPreDisconnect();
	DoDisconnect();
	DoPostDisconnect();
	m_connected = false;
}

void Database::PrepareSTMT(std::string&& name, std::string&& query) noexcept {
	std::unique_ptr<PreparedSTMT> prepared = CreatePreparedSTMT(std::move(name), std::move(query));
	if (prepared)
	m_prepared_stmts.insert({prepared->Name(), std::move(prepared)});
}