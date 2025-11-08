export module spt.infrastructure:repository;

import std;
import :database;

namespace spt::infrastructure::repositories {
    using std::format;
    using std::getenv;
    using std::string;
    using std::string_view;
    using spt::infrastructure::sql::Database;

    export class Repository {
        private:
            Database _db;

        protected:
            Database& getDB() {
                return _db;
            }

            const Database& getDB() const {
                return _db;
            }

            virtual void ensureSchema() = 0;

        public:
            Repository() 
                : _db { format("{0}\\Blendwerk\\SPT\\spt.db", getenv("USERPROFILE")) } {
            }

            Repository(const Repository&) = delete;
            Repository(Repository&&) = delete;
            virtual ~Repository() = default;        
    };
}