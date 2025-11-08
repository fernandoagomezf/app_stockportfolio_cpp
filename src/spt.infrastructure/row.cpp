export module spt.infrastructure:row;

import std;
import :value;

namespace spt::infrastructure::sql {
    using std::map;
    using std::out_of_range;
    using std::size_t;
    using std::string;
    using std::string_view;
    using spt::infrastructure::sql::Value;

    export class Row {
        public:
            size_t count() const {
                return _columns.size();
            }

            bool contains(const string& name) const {
                return _columns.contains(name);
            }
            const Value& get(const string& name) const {
                auto it = _columns.find(name);
                if (it == _columns.end()) {
                    throw out_of_range {
                        format("Column not found: {0}", name)
                    };
                }
                return it->second;
            }

            void set(const string& name, Value value) {
                _columns[name] = value;
            }

        private:
            map<string, Value> _columns;
    };
}