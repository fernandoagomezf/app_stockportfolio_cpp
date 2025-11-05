export module spt.infrastructure.sqlite:database;

import <sqlite3.h>;
import std;
import :statement;

namespace spt::infrastructure::sql   {
    using std::string;
    using std::string_view;
    using std::unique_ptr;
    using std::vector;
    using spt::infrastructure::sql::Statement;

    export class Database final {
        public:
            Database(string_view name);
            Database(Database const&) = delete;
            Database(Database&&) = delete;
            ~Database();
            
            Database& operator=(Database const&) = delete;
            Database& operator=(Database&&) = delete;

            Statement prepare(string_view sql);
            void begin();
            void commit();
            void rollback();
            
            void execute(string_view sql);
            void execute(string_view sql, initializer_list<Value> params);
            ResultSet query(string_view sql);
            ResultSet query(string_view sql, initializer_list<SqlValue> params);            

        private:
            struct Deleter {
                void operator()(sqlite3* db) const;
            };

            unique_ptr<sqlite3, Deleter> _db;
            bool _isInTransaction;
    };
}