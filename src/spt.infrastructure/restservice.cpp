export module spt.infrastructure:restservice;

import std;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;

namespace spt::infrastructure::services {
    using std::format;
    using std::nullopt;
    using std::optional;
    using std::runtime_error;
    using std::string;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::text::JsonValue;

    export class RestService {
        private:
            string _userAgent;
            string _accept;

        public:
            RestService()
                : _userAgent { "Blendwerk SPT/1.0" },
                  _accept { "application/json" }
            {
            }

            string getAccept() const {
                return _accept;
            }

            string getUserAgent() const {
                return _userAgent;
            }

            virtual ~RestService() = default;            
            
        protected:
            void setAccept(const string& accept) {
                _accept = accept;
            }

            void setUserAgent(const string& userAgent) {
                _userAgent = userAgent;
            }

            JsonValue fetchData(string url) {
                HttpRequest request { url, HttpMethod::GET };
                request.setHeader("Accept", _accept);
                request.setHeader("User-Agent", _userAgent);

                HttpClient client { };
                client.timeout(10L);

                HttpResponse response { client.send(request) };
                if (!response.isSuccess()) {
                    throw runtime_error {
                        format("Failed to fetch data from REST Service: status code {0}", response.status())
                    };
                }

                JsonValue json { JsonParser::parse(response.body()) };                    
                if (!json.isObject()) {
                    throw runtime_error { "Invalid response from REST Service." };
                }

                return json;
            }
    };
}