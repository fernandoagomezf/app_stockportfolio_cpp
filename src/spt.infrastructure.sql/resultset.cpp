module spt.infrastructure.sql:resultset;

import std;
import :row;
import :resultset;

using std::move;
using std::size_t;
using spt::infrastructure::sql::Row;
using spt::infrastructure::sql::ResultSet;

size_t ResultSet::count() const {
    return _rows.size();
}

void ResultSet::add(Row row) {
    _rows.push_back(move(row));
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