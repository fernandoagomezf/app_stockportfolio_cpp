export module spt.infrastructure:resultset;

import std;
import :row;

namespace spt::infrastructure::sql {
    using std::move;
    using std::size_t;
    using std::vector;
    using spt::infrastructure::sql::Row;

    export class ResultSet {
        public:
            using const_iterator = vector<Row>::const_iterator;

            size_t count() const {
                return _rows.size();
            }

            void add(Row row) {
                _rows.push_back(move(row));
            }

            void clear() {
                _rows.clear();
            }

            const_iterator begin() const {
                return _rows.cbegin();
            }

            const_iterator end() const {
                return _rows.cend();
            }

        private:
            vector<Row> _rows;
    };
}