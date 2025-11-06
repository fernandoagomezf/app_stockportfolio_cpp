module spt.infrastructure.net:httpheaders;

import std;
import :httpheaders;

using std::string;
using std::string_view;
using spt::infrastructure::net::HttpHeaders;

bool HttpHeaders::contains(string_view key) const {
    return _headers.contains(string { key });
}

string HttpHeaders::get(string_view key) const {
    string result { };
    auto it = _headers.find(string { key });
    if (it != _headers.end()) {
        result = it->second;
    }

    return result;
}

void HttpHeaders::set(string_view key, string_view value) {
    _headers[string { key }] = string { value };
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