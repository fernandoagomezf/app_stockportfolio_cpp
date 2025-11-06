export module spt.infrastructure.net:httpresponse;

import std;
import :httpheaders;

namespace spt::infrastructure::net {
    using std::string;
    using std::string_view;
    using spt::infrastructure::net::HttpHeaders;

    export class HttpResponse final {
        public:
            HttpResponse(long status, string_view body, const HttpHeaders& headers);

            bool isSuccess() const;
            long status() const;
            string_view body() const;
            const HttpHeaders& headers() const;

        private:
            long _status;
            string _body;
            HttpHeaders _headers;

    };
}