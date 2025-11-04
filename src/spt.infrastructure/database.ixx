export module spt.infrastructure:database;

import <sqlite3.h>;
import std;

namespace spt::infrastructure {
    using std::string;
    using std::string_view;
    using std::unique_ptr;

    export class Database {
        public:
            Database(string_view name);
            Database(Database const&) = delete;
            Database(Database&&) = delete;
            ~Database();
            
            Database& operator=(Database const&) = delete;
            Database& operator=(Database&&) = delete;

        private:
            struct Deleter {
                void operator()(sqlite3* db) const;
            };
            
            unique_ptr<sqlite3, Deleter> _db;
    };
}