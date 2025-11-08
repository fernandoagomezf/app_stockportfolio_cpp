export module spt.infrastructure:httprequest;

import std;
import :httpheaders;

namespace spt::infrastructure::net {
    using std::map;
    using std::string;
    using std::string_view;

    export enum class HttpMethod {
        GET,
        POST,
        PUT,
        DEL
    };

    export class HttpRequest final {        
        private:
            string _url;
            HttpMethod _method;
            string _body;
            HttpHeaders _headers;

        public:
            HttpRequest(string_view url, HttpMethod method) 
                : _url { url }, 
                  _method { method },
                  _body { },
                  _headers { }
            {
            }

            string_view url() const {
                return _url;
            }

            HttpMethod method() const {
                return _method;
            }

            string_view body() const {
                return _body;
            }

            const HttpHeaders& headers() const {
                return _headers;
            }

            void setHeader(const string& key, const string& value) {
                _headers.set(key, value);
            }

            void setBody(const string& body) {
                _body = body;
            }

    };
}