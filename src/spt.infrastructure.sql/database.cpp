export module spt.infrastructure.sql:database;

import <sqlite3.h>;
import std;
import :statement;

namespace spt::infrastructure::sql   {
    using std::filesystem::path;
    using std::format;
    using std::initializer_list;
    using std::invalid_argument;
    using std::logic_error;
    using std::runtime_error;
    using std::string;
    using std::string_view;
    using std::unique_ptr;
    using std::vector;
    using spt::infrastructure::sql::Statement;

    export class Database final {
        private:        
            struct Deleter {
                void operator()(sqlite3* db) const {
                    if (db != nullptr) {
                        sqlite3_close_v2(db);
                    }
                }
            };
            unique_ptr<sqlite3, Deleter> _db;
            bool _isInTransaction;

            static void init(string_view name) {
                path file { name };
                path dir { file.parent_path() };
                if (!exists(dir)) {
                    create_directories(dir);
                }
            }

        public:
            Database(string_view name)
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

            Database(Database const&) = delete;
            Database(Database&&) = delete;

            ~Database() {
                if (_isInTransaction) {
                    try {
                        rollback();
                    } catch (...) {
                        // swallow exceptions if rollback fails
                    }
                }
            }
            
            Database& operator=(Database const&) = delete;
            Database& operator=(Database&&) = delete;

            Statement prepare(string_view sql) {
                return Statement(_db.get(), sql);
            }

            void begin() {
                if (_isInTransaction) {
                    throw logic_error { "Transaction already in progress" };
                }
                execute("BEGIN TRANSACTION");
                _isInTransaction = true;
            }

            void commit() {
                if (!_isInTransaction) {
                    throw logic_error { "No transaction in progress" };
                }
                execute("COMMIT");
                _isInTransaction = false;
            }

            void rollback() {
                if (_isInTransaction) {
                    execute("ROLLBACK");
                    _isInTransaction = false;
                }
            }
            
            void execute(string_view sql) {
                char* msg = nullptr;
                int rc = sqlite3_exec(_db.get(), sql.data(), nullptr, nullptr, &msg);
                
                if (rc != SQLITE_OK) {
                    string error { msg != nullptr ? msg : "Unknown error" };
                    sqlite3_free(msg);
                    throw runtime_error {
                        format("SQL execution failed: {}", error)
                    };
                }
            }

            void execute(string_view sql, initializer_list<Value> paramsList) {
                vector<Value> params { paramsList };
                Statement stmt { prepare(sql) };
                int idx { 0 };

                for (const auto& param : params) {
                    ++idx;
                    stmt.bind(idx, param);
                }

                stmt.execute();
            }

            ResultSet query(string_view sql) {
                Statement stmt { prepare(sql) };
                return stmt.executeQuery();
            }

            ResultSet query(string_view sql, initializer_list<Value> paramsList) {
                vector<Value> params { paramsList };
                Statement stmt { prepare(sql) };
                int idx { 0 };

                for (const auto& param : params) {
                    ++idx;
                    stmt.bind(idx, param);
                }

                return stmt.executeQuery();
            }        
    };
}