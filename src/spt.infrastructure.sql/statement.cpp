module;

#include <sqlite3.h>

module spt.infrastructure.sql:statement;

import std;
import :resultset;
import :row;
import :statement;
import :value;

using std::byte;
using std::format;
using std::monostate;
using std::move;
using std::runtime_error;
using std::size_t;
using std::span;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;
using std::visit;
using spt::infrastructure::sql::ResultSet;
using spt::infrastructure::sql::Row;
using spt::infrastructure::sql::Statement;
using spt::infrastructure::sql::Value;

Statement::Statement(sqlite3* db, string_view sql) {
    sqlite3_stmt* stmt { nullptr };
    int rc = sqlite3_prepare_v2(db, sql.data(), static_cast<int>(sql.size()), &stmt, nullptr);    
    if (rc != SQLITE_OK) {
        auto err = sqlite3_errmsg(db);
        throw runtime_error { format("Failed to prepare statement: {0}", err) };
    }
    
    _stmt.reset(stmt);
}

void Statement::Deleter::operator()(sqlite3_stmt* stmt) const {
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }
}

void Statement::bind(int index, std::nullptr_t) {
    sqlite3_bind_null(_stmt.get(), index);
}

void Statement::bind(int index, int value) {
    sqlite3_bind_int(_stmt.get(), index, value);
}

void Statement::bind(int index, long long value) {
    sqlite3_bind_int64(_stmt.get(), index, value);
}

void Statement::bind(int index, double value) {
    sqlite3_bind_double(_stmt.get(), index, value);
}

void Statement::bind(int index, string_view value) {
    sqlite3_bind_text(_stmt.get(), index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
}

void Statement::bind(int index, vector<byte> value) {
    sqlite3_bind_blob(_stmt.get(), index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
}

void Statement::bind(int index, Value value) {
    if (value.isNull()) {
        bind(index, nullptr);
    } else if (value.isInt()) {
        bind(index, value.getInt());
    } else if (value.isLong()) {
        bind(index, value.getLong());
    } else if (value.isDouble()) {
        bind(index, value.getDouble());
    } else if (value.isString()) {
        bind(index, string_view(value.getString()));
    } else if (value.isBlob()) {
        bind(index, value.getBlob());
    }
}

void Statement::execute() {
    int rc = sqlite3_step(_stmt.get());    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        throw runtime_error { 
            format("Statement execution failed: {0}", sqlite3_errstr(rc))
        };
    }
}

ResultSet Statement::executeQuery() {
    ResultSet result { };
    
    int rc;
    do {
        rc = sqlite3_step(_stmt.get());        
        if (rc == SQLITE_DONE) {
            break;
        }
        
        if (rc != SQLITE_ROW) {
            throw runtime_error {
                format("Query execution failed: {0}", sqlite3_errstr(rc))
            };
        }
        
        Row row { };
        auto count = sqlite3_column_count(_stmt.get());        
        for (auto i = 0; i < count; ++i) {
            string columnName { sqlite3_column_name(_stmt.get(), i) };
            int columnType { sqlite3_column_type(_stmt.get(), i) };
            
            Value value { };            
            switch (columnType) {
                case SQLITE_INTEGER:
                    value = { sqlite3_column_int64(_stmt.get(), i) };
                    break;                    
                case SQLITE_FLOAT:
                    value = { sqlite3_column_double(_stmt.get(), i) };
                    break;                    
                case SQLITE_TEXT: {
                    const unsigned char* text = sqlite3_column_text(_stmt.get(), i);
                    value = { string { reinterpret_cast<const char*>(text) } };
                    break;
                }                
                case SQLITE_BLOB: {
                    const void* blob = sqlite3_column_blob(_stmt.get(), i);
                    int size = sqlite3_column_bytes(_stmt.get(), i);
                    if (size > 0 && blob != nullptr) {
                        span<const byte> span { 
                            static_cast<const byte*>(blob), 
                            static_cast<size_t>(size)
                        };
                        vector<byte> data {
                            span.begin(), 
                            span.end()
                        };
                        value = { move(data) };
                    }
                    break;
                }
            }
            
            row.set(columnName, move(value));
        }
        
        result.add(move(row));
    } while (rc != SQLITE_DONE);
    
    return move(result);
}

void Statement::reset() {
    sqlite3_reset(_stmt.get());
    sqlite3_clear_bindings(_stmt.get());
}
