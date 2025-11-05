export module spt.infrastructure.repositories:repository;

import std;
import spt.infrastructure.sql;

namespace spt::infrastructure::repositories {
    using std::string;
    using std::string_view;
    using spt::infrastructure::sql::Database;

    export class Repository {
        public:
            Repository();
            Repository(const Repository&) = delete;
            Repository(Repository&&) = delete;
            virtual ~Repository() = default;

        protected:
            Database& getDB();
            const Database& getDB() const;


        private:
            Database _db;
    };
}