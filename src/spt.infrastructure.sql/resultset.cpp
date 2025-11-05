module spt.infrastructure.sql:resultset;

import std;
import :row;
import :resultset;

using std::size_t;
using spt::infrastructure::sql::Row;
using spt::infrastructure::sql::ResultSet;

ResultSet::ResultSet(size_t count) {
    _rows.reserve(count);
}

size_t ResultSet::count() const {
    return _rows.size();
}

void ResultSet::add(Row row) {
    _rows.push_back(row);
}

void ResultSet::clear() {
    _rows.clear();
}

ResultSet::const_iterator ResultSet::begin() const {
    return _rows.cbegin();
}

ResultSet::const_iterator ResultSet::end() const {
    return _rows.cend();
}