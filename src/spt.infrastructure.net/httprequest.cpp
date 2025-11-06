module spt.infrastructure.net:httprequest;

import std;
import :httprequest;

using std::invalid_argument;
using std::map;
using std::move;
using std::string;
using std::string_view;
using spt::infrastructure::net::HttpHeaders;
using spt::infrastructure::net::HttpRequest;
using spt::infrastructure::net::HttpMethod;

HttpRequest::HttpRequest(string_view url, HttpMethod method)
    : _url { url }, 
      _method { method },
      _body { },
      _headers { }
{
}

string_view HttpRequest::url() const {
    return _url;
}

HttpMethod HttpRequest::method() const {
    return _method;
}

string_view HttpRequest::body() const {
    return _body;
}

const HttpHeaders& HttpRequest::headers() const {
    return _headers;
}

void HttpRequest::setHeader(string_view key, string_view value) {
    _headers.set(key, value);
}

void HttpRequest::setBody(string_view body) {
    _body = string { body };
}

