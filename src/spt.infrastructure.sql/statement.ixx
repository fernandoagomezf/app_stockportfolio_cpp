export module spt.infrastructure.sql:statement;

import std;
import :value;
import :resultset;
import <sqlite3.h>;

namespace spt::infrastructure::sql {
    using std::byte;
    using std::string;
    using std::string_view;
    using std::unique_ptr;
    using std::vector;
    using spt::infrastructure::sql::ResultSet;
    using spt::infrastructure::sql::Value;

    export class Statement final {
        friend class Database;

        public:                   
            Statement(const Statement&) = delete;
            Statement(Statement&&) = default;
            Statement& operator=(const Statement&) = delete;
            Statement& operator=(Statement&&) = default;

            void bind(int index, std::nullptr_t);
            void bind(int index, int value);
            void bind(int index, long long value);
            void bind(int index, double value);
            void bind(int index, string_view value);
            void bind(int index, const vector<byte>& value);
            void bind(int index, const Value& value);
            
            void execute();
            ResultSet executeQuery();
            void reset();

        private:
            struct Deleter {
                void operator()(sqlite3_stmt* stmt) const;
            };

            unique_ptr<sqlite3_stmt, Deleter> _stmt;
            
            Statement(sqlite3* db, string_view sql);
    };
}