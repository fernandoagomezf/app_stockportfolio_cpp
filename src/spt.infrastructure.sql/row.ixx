export module spt.infrastructure.sql:row;

import std;
import :value;

namespace spt::infrastructure::sql {
    using std::map;
    using std::size_t;
    using std::string;
    using std::string_view;
    using spt::infrastructure::sql::Value;

    export class Row {
        public:
            size_t count() const;
            bool contains(string name) const;
            Value get(string name) const;
            void set(string name, Value value);

        private:
            map<string, Value> _columns;
    };
}