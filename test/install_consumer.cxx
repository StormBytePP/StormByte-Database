#include <StormByte/database/value.hxx>

#if defined(STORMBYTE_TEST_SQLITE)
#include <StormByte/database/sqlite/sqlite3.hxx>
#endif

class ConsumerDatabase
#if defined(STORMBYTE_TEST_SQLITE)
	: public StormByte::Database::SQLite::SQLite3
#endif
{
#if defined(STORMBYTE_TEST_SQLITE)
	public:
		ConsumerDatabase() : SQLite3(StormByte::Shared<StormByte::Logger::Log>{}) {}
#endif
};

int main() {
	StormByte::Database::Value value{42};
	if (value.Get<int>() != 42)
		return 1;

#if defined(STORMBYTE_TEST_SQLITE)
	ConsumerDatabase db;
	if (!db.Connect())
		return 2;
	const auto rows = db.Query("SELECT 42;");
	if (!rows || rows->Count() != 1 || rows->operator[](0)[0].Get<int>() != 42)
		return 3;
	auto transaction = db.BeginTransaction();
	if (!transaction)
		return 4;
	transaction->Rollback();
#endif
	return 0;
}
