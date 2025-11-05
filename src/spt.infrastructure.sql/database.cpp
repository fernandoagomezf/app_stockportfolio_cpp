module;

#include <sqlite3.h>

module spt.infrastructure.sql:database;

import std;
import :database;
import :statement;
import :value;
import :row;

using std::filesystem::create_directories;
using std::filesystem::exists;
using std::filesystem::filesystem_error;
using std::filesystem::path;
using std::format;
using std::getenv;
using std::initializer_list;
using std::invalid_argument;
using std::logic_error;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector; 
using spt::infrastructure::sql::Database;
using spt::infrastructure::sql::Statement;
using spt::infrastructure::sql::Value;
using spt::infrastructure::sql::Row;
using spt::infrastructure::sql::ResultSet;

Database::Database(string_view name) 
    : _isInTransaction { false }
{
    init(name);
    sqlite3* db { nullptr };
    int rc = sqlite3_open_v2(name.data(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        auto err = sqlite3_errmsg(db);
        throw invalid_argument { 
            format("Failed to open database: {0}", err) 
        };
    }
    _db.reset(db);
}

Database::~Database() {
    if (_isInTransaction) {
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


void Database::init(string_view name) {    
    path file { name };
    path dir { file.parent_path() };
    if (!exists(dir)) {
        create_directories(dir);
    }
}

Statement Database::prepare(string_view sql) {
    return Statement(_db.get(), sql);
}

void Database::begin() {
    if (_isInTransaction) {
        throw logic_error { "Transaction already in progress" };
    }
    execute("BEGIN TRANSACTION");
    _isInTransaction = true;
}

void Database::commit() {
    if (!_isInTransaction) {
        throw logic_error { "No transaction in progress" };
    }
    execute("COMMIT");
    _isInTransaction = false;
}

void Database::rollback() {
    if (_isInTransaction) {
        execute("ROLLBACK");
        _isInTransaction = false;
    }
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
    int idx { 0 };

    for (const auto& param : params) {
        ++idx;
        stmt.bind(idx, param);
    }

    stmt.execute();
}

ResultSet Database::query(string_view sql) {
    Statement stmt { prepare(sql) };
    return stmt.executeQuery();
}

ResultSet Database::query(string_view sql, initializer_list<Value> paramsList) {
    vector<Value> params { paramsList };
    Statement stmt { prepare(sql) };
    int idx { 0 };

    for (const auto& param : params) {
        ++idx;
        stmt.bind(idx, param);
    }

    return stmt.executeQuery();
}
