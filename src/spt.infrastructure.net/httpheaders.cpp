module spt.infrastructure.net:httpheaders;

import std;
import :httpheaders;

using std::nullopt;
using std::optional;
using std::string;
using std::string_view;
using spt::infrastructure::net::HttpHeaders;

bool HttpHeaders::contains(const string& key) const {
    return _headers.contains(key);
}

optional<string> HttpHeaders::get(const string& key) const {
    optional<string> result { nullopt };
    auto it = _headers.find(key);
    if (it != _headers.end()) {
        result = it->second;
    }

    return result;
}

void HttpHeaders::set(const string& key, const string& value) {
    _headers[key] = value;
}

HttpHeaders::iterator HttpHeaders::begin() {
    return _headers.begin();
}

HttpHeaders::iterator HttpHeaders::end() {
    return _headers.end();
}

HttpHeaders::const_iterator HttpHeaders::begin() const {
    return _headers.begin();
}

HttpHeaders::const_iterator HttpHeaders::end() const {
    return _headers.end();
}