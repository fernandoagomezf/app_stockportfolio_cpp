export module spt.infrastructure.net:httpresponse;

import std;
import :httpheaders;

namespace spt::infrastructure::net {
    using std::string;
    using std::string_view;
    using spt::infrastructure::net::HttpHeaders;

    export class HttpResponse final {
        private:
            long _status;
            string _body;
            HttpHeaders _headers;

        public:
            HttpResponse(long status, string_view body, const HttpHeaders& headers) 
                : _status { status },
                  _body { body },
                  _headers { headers } 
            {
            }

            bool isSuccess() const {
                return _status >= 200 && _status < 300;
            }

            long status() const {
                return _status;
            }

            string_view body() const {
                return _body;
            }

            const HttpHeaders& headers() const {
                return _headers;
            }
    };
}