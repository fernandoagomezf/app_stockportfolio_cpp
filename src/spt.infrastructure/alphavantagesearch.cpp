export module spt.infrastructure:alphavantagesearch;

import std;
import spt.domain;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;
import :alphavantageservice;

namespace spt::infrastructure::services {
    using std::format;
    using std::make_format_args;
    using std::nullopt;
    using std::move;
    using std::optional;
    using std::string;
    using std::string_view;
    using std::runtime_error;
    using std::toupper;
    using std::transform;
    using spt::domain::investments::Company;
    using spt::domain::investments::CompanySearch;
    using spt::domain::investments::Ticker;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::text::JsonValue;
    using spt::infrastructure::services::AlphaVantageService;

    export class AlphaVantageSearch final : public AlphaVantageService, public CompanySearch {
        public:
            optional<Company> search(Ticker ticker) override {
                return search(ticker.symbol());
            }

            optional<Company> search(string name) override {
                optional<Company> result { nullopt };

                string url { 
                    format("{0}apiKey={1}&function=SYMBOL_SEARCH",
                        getBaseUrl(),
                        getApiKey()
                    )
                };

                JsonValue json { fetchData(url) };
                if (json.contains("bestMatches") && json["bestMatches"].isArray()) {
                    const auto& matches = json["bestMatches"].getArray();
                    if (!matches.empty()) {
                        const auto& bestMatch = matches[0];
                        if (bestMatch.isObject()) {
                            Ticker ticker { bestMatch["1. symbol"].getString() };
                            Company company { move(ticker) };
                            company.setName(bestMatch["2. name"].getString());
                            company.setType(bestMatch["3. type"].getString());
                            company.setRegion(bestMatch["4. region"].getString());
                            company.setCurrency(bestMatch["8. currency"].getString());
                            result = move(company);
                        }
                    }
                }

                return result;
            }

        private:            
            Company get(string_view searchTerm) {
                string url { 
                    format("{0}apiKey={1}&function=SYMBOL_SEARCH",
                        getBaseUrl(),
                        getApiKey()
                    )
                };

                JsonValue json { fetchData(url) };
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