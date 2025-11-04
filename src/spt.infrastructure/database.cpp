module;

#include <sqlite3.h>

module spt.infrastructure:database;

import std;
import :database;

using std::make_unique;
using std::invalid_argument;
using std::string_view;
using std::unique_ptr;
using spt::infrastructure::Database;

Database::Database(string_view name)
{
    sqlite3* db { nullptr };
    int rc = sqlite3_open_v2(name.data(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        throw invalid_argument { "Failed to open database." };
    }
    _db.reset(db);
}

void Database::Deleter::operator()(sqlite3* db) const {
    if (db != nullptr) {
        sqlite3_close_v2(db);
    }
}

Database::~Database()
{
    // unique_ptr with custom deleter will automatically call sqlite3_close_v2
}