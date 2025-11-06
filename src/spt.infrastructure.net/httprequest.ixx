export module spt.infrastructure.net:httprequest;

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
        public:
            HttpRequest(string_view url, HttpMethod method);

            string_view url() const;
            HttpMethod method() const;
            string_view body() const;
            const HttpHeaders& headers() const;

            void setHeader(const string& key, const string& value);
            void setBody(const string& body);

        private:
            string _url;
            HttpMethod _method;
            string _body;
            HttpHeaders _headers;
    };
}