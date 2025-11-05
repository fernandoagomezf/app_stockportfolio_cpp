export module spt.infrastructure.sql:resultset;

import std;
import :row;

namespace spt::infrastructure::sql {
    using std::size_t;
    using std::vector;
    using spt::infrastructure::sql::Row;

    export class ResultSet {
        public:
            using const_iterator = vector<Row>::const_iterator;

            size_t count() const;
            void add(Row row);
            void clear();

            const_iterator begin() const;
            const_iterator end() const;

        private:
            vector<Row> _rows;
    };
}