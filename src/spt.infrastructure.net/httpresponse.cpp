module spt.infrastructure.net:httpresponse;

import std;
import :httpheaders;
import :httpresponse;

using std::invalid_argument;
using std::map;
using std::move;
using std::string;
using std::string_view;
using spt::infrastructure::net::HttpHeaders;
using spt::infrastructure::net::HttpResponse;

HttpResponse::HttpResponse(long status, string_view body, const HttpHeaders& headers)
    : _status { status },
      _body { body }
{
}

bool HttpResponse::isSuccess() const {
    return _status >= 200 && _status < 300;
}

long HttpResponse::status() const {
    return _status;
}

string_view HttpResponse::body() const {
    return _body;
}

const HttpHeaders& HttpResponse::headers() const {
    return _headers;
}