export module spt.infrastructure.sql:statement;

import std;
import :value;
import :resultset;
import <sqlite3.h>;

namespace spt::infrastructure::sql {
    using std::byte;
    using std::nullptr_t;
    using std::format;
    using std::move;
    using std::runtime_error;
    using std::size_t;
    using std::span;
    using std::string;
    using std::string_view;
    using std::unique_ptr;
    using std::vector;
    using spt::infrastructure::sql::ResultSet;
    using spt::infrastructure::sql::Value;

    export class Statement final {
        friend class Database;

        private:
            struct Deleter {
                void operator()(sqlite3_stmt* stmt) const {
                    if (stmt != nullptr) {
                        sqlite3_finalize(stmt);
                    }
                }
            };

            unique_ptr<sqlite3_stmt, Deleter> _stmt;
            
            Statement(sqlite3* db, string_view sql) {
                sqlite3_stmt* stmt { nullptr };
                int rc = sqlite3_prepare_v2(db, sql.data(), static_cast<int>(sql.size()), &stmt, nullptr);    
                if (rc != SQLITE_OK) {
                    auto err = sqlite3_errmsg(db);
                    throw runtime_error { 
                        format("Failed to prepare statement: {0}", err) 
                    };
                }
                
                _stmt.reset(stmt);
            }

        public:                   
            Statement(const Statement&) = delete;
            Statement(Statement&&) = default;
            Statement& operator=(const Statement&) = delete;
            Statement& operator=(Statement&&) = default;

            void bind(int index, nullptr_t) {
                sqlite3_bind_null(_stmt.get(), index);
            }

            void bind(int index, int value) {
                sqlite3_bind_int(_stmt.get(), index, value);
            }

            void bind(int index, long long value) {
                sqlite3_bind_int64(_stmt.get(), index, value);
            }

            void bind(int index, double value) {
                sqlite3_bind_double(_stmt.get(), index, value);
            }

            void bind(int index, string_view value) {
                sqlite3_bind_text(_stmt.get(), index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
            }

            void bind(int index, const vector<byte>& value) {
                sqlite3_bind_blob(_stmt.get(), index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
            }

            void bind(int index, const Value& value) {
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
            
            void execute() {
                int rc = sqlite3_step(_stmt.get());    
                if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                    throw runtime_error { 
                        format("Statement execution failed: {0}", sqlite3_errstr(rc))
                    };
                }
            }

            ResultSet executeQuery() {
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
                            case SQLITE_NULL:
                                value = {};
                                break;
                        }
                        
                        row.set(columnName, move(value));
                    }
                    
                    result.add(move(row));
                } while (rc != SQLITE_DONE);
                
                return move(result);
            }

            void reset() {
                sqlite3_reset(_stmt.get());
                sqlite3_clear_bindings(_stmt.get());
            }
    };
}