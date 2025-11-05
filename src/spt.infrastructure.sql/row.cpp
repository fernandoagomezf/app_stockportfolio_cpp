module spt.infrastructure.sql:row;

import std;
import :value;
import :row;

using std::format;
using std::map;
using std::out_of_range;
using std::size_t;
using std::string;
using std::string_view;
using spt::infrastructure::sql::Value;
using spt::infrastructure::sql::Row;

size_t Row::count() const {
    return _columns.size();
}

bool Row::contains(string_view name) const {
    return _columns.contains(string { name });
}

const Value& Row::get(string_view name) const {
    auto it = _columns.find(string { name });
    if (it == _columns.end()) {
        throw out_of_range {
            format("Column not found: {0}", string { name })
        };
    }
    return it->second;
}

void Row::set(const string& name, Value value) {
    _columns[name] = value;
}


