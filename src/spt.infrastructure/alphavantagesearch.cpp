export module spt.infrastructure:alphavantagesearch;

import std;
import spt.domain;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;

namespace spt::infrastructure::services {
    using std::format;
    using std::make_format_args;
    using std::move;
    using std::optional;
    using std::string;
    using std::string_view;
    using std::runtime_error;
    using std::toupper;
    using std::transform;
    using std::vformat;
    using spt::domain::investments::Company;
    using spt::domain::investments::CompanySearch;
    using spt::domain::investments::Ticker;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::text::JsonValue;

    export class AlphaVantageSearch final : public CompanySearch {
        public:
            AlphaVantageSearch()
                : _apiKey { },
                  _urlTemplate { }                 
            {
                loadSettings();
            }

            optional<Company> search(Ticker ticker) override {
                return get(ticker.symbol());
            }

            optional<Company> search(string name) override {      
                return get(name);
            }

        private:
            string _apiKey;
            string _urlTemplate;

            void loadSettings() {
                _urlTemplate = "https://www.alphavantage.co/query?function=SYMBOL_SEARCH&keywords={0}&apikey={1}";
                _apiKey = "2SACRS4UK8QY45Z9";
            }

            Company get(string_view searchTerm) {
                string url { 
                    vformat(_urlTemplate, make_format_args(searchTerm, _apiKey)) 
                };

                HttpRequest request { url, HttpMethod::GET };
                request.setHeader("Accept", "application/json");
                request.setHeader("User-Agent", "Blendwerk SPT/1.0");

                HttpClient client { };
                client.timeout(10L);

                HttpResponse response = client.send(request);                    
                if (!response.isSuccess()) {
                    throw runtime_error {
                        format("Failed to fetch data from Alpha Vantage: HTTP {0}", response.status())
                    };
                }

                JsonValue json { JsonParser::parse(response.body()) };                    
                if (!json.isObject()) {
                    throw runtime_error { "Invalid response from Alpha Vantage." };
                }

                if (!json.contains("bestMatches") || !json["bestMatches"].isArray()) {
                    throw runtime_error { 
                        format("No results found for '{0}'.", searchTerm) 
                    };
                }

                const auto& matches = json["bestMatches"].getArray();
                if (matches.empty()) {
                    throw runtime_error { 
                        format("No results found for '{0}'.", searchTerm) 
                    };
                }

                const auto& bestMatch = matches[0];
                if (!bestMatch.isObject()) {
                    throw runtime_error { 
                        format("Invalid data format received from Alpha Vantage for '{0}'.", searchTerm) 
                    };
                }
                
                Ticker ticker { bestMatch["1. symbol"].getString() };
                Company company { move(ticker) };
                company.setName(bestMatch["2. name"].getString());
                company.setType(bestMatch["3. type"].getString());
                company.setRegion(bestMatch["4. region"].getString());
                company.setCurrency(bestMatch["8. currency"].getString());

                return move(company);
            }
    };
}