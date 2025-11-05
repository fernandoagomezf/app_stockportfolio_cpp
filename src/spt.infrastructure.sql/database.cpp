module;

#include <sqlite3.h>

module spt.infrastructure:database;

import std;
import :database;
import :statement;

using std::expected;
using std::format;
using std::initializer_list;
using std::invalid_argument;
using std::make_unique;
using std::monostate;
using std::runtime_error;
using std::string;
using std::string_view;
using std::unexpected;
using std::unique_ptr;
using std::unordered_map;
using std::vector;
using std::visit;
using spt::infrastructure::Database;
using spt::infrastructure::Statement;
using spt::infrastructure::Value;
using spt::infrastructure::Row;
using spt::infrastructure::ResultSet;

Database::Database(string_view name) 
    : _inTransaction { false }
{
    sqlite3* db { nullptr };
    int rc = sqlite3_open_v2(name.data(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        auto err = sqlite3_errmsg(db);
        throw invalid_argument { format("Failed to open database: {0}", err) };
    }
    _db.reset(db);
}

Database::~Database() {
    if (_inTransaction) {
        try {
            rollback();
        } catch (...) {
            // swallow exceptions if rollback fails
        }
    }
}

void Database::Deleter::operator()(sqlite3* db) const {
    if (db != nullptr) {
        sqlite3_close_v2(db);
    }
}

Statement Database::prepare(string_view sql) {
    return Statement(_db.get(), sql);
}

void Database::begin() {
    execute("BEGIN TRANSACTION");
    _inTransaction = true;
}

void Database::commit() {
    execute("COMMIT");
    _inTransaction = false;
}

void Database::rollback() {
    execute("ROLLBACK");
    _inTransaction = false;
}

void Database::execute(string_view sql) {
    char* msg = nullptr;
    int rc = sqlite3_exec(_db.get(), sql.data(), nullptr, nullptr, &msg);
    
    if (rc != SQLITE_OK) {
        string error = msg != nullptr ? msg : "Unknown error";
        sqlite3_free(msg);
        throw runtime_error {
            format("SQL execution failed: {}", error)
        };
    }
}

void Database::execute(string_view sql, initializer_list<Value> paramsList) {
    vector<Value> params { paramsList };
    Statement stmt { prepare(sql) };
    int idx { 0 }

    for (auto param; : params) {
        stmt.bind(idx, param);
        ++idx;
    }

    stmt.execute();
}

ResultSet Database::query(string_view sql) {
    Statement stmt { prepare(sql) };
    return stmt.executeQuery();
}

ResultSet Database::query(string_view sql, initializer_list<SqlValue> paramsList) {
    vector<SqlValue> params { paramsList };
    Statement stmt { prepare(sql) };
    int idx { 0 };

    for (auto param : params) {
        stmt.bind(idx, param);
        ++idx;
    }

    return stmt.executeQuery();
}


