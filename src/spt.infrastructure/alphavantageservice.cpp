export module spt.infrastructure:alphavantageservice;

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

    export class AlphaVantageService {
        private:
            string _baseUrl;
            string _apiKey;

        public:
            AlphaVantageService()
                : _baseUrl { },
                  _apiKey { } 
            {
                loadSettings();
            }

            virtual ~AlphaVantageService() = default;            
            
        protected:
            string getApiKey() const {
                return _apiKey;
            }

            string getBaseUrl() const {
                return _baseUrl;
            }

            JsonValue fetchData(string url) {
                HttpRequest request { url, HttpMethod::GET };
                request.setHeader("Accept", "application/json");
                request.setHeader("User-Agent", "Blendwerk SPT/1.0");

                HttpClient client { };
                client.timeout(10L);

                HttpResponse response { client.send(request) };
                if (!response.isSuccess()) {
                    throw runtime_error {
                        format("Failed to fetch data from Alpha Vantage Service: status code {0}", response.status())
                    };
                }

                JsonValue json { JsonParser::parse(response.body()) };                    
                if (!json.isObject()) {
                    throw runtime_error { "Invalid response from Alpha Vantage." };
                }
            }

        private:
            void loadSettings() {
                _baseUrl = "https://www.alphavantage.co/query?";
                _apiKey = "2SACRS4UK8QY45Z9";
            }
    };
}