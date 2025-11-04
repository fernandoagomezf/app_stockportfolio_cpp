module spt.infrastructure.sql:row;

import std;
import :value;
import :row;

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

bool Row::contains(string name) const {
    auto it = _columns.find(name);
    return it != _columns.end();
}

Value Row::get(string name) const {
    auto it = _columns.find(string{name});
    if (it == _columns.end()) {
        throw out_of_range("Column not found: " + string{name});
    }
    return it->second;
}

void Row::set(string name, Value value) {
    _columns[name] = value;
}


