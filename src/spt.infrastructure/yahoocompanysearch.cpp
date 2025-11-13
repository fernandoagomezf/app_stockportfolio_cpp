export module spt.infrastructure:yahoocompanysearch;

import std;
import spt.domain;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;
import :restservice;

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
    using spt::infrastructure::services::RestService;

    export class YahooCompanySearch final : public RestService, public CompanySearch {
        private:
            string _url;

        public:
            YahooCompanySearch()
                : RestService(),
                  _url { "https://query1.finance.yahoo.com/v1/finance/search" }
            {
            }

            optional<Company> search(Ticker ticker) override {
                return search(ticker.symbol());
            }

            optional<Company> search(string name) override {
                optional<Company> result { nullopt };

                string url { 
                    format("{0}?q={1}", _url, name)
                };

                JsonValue json { fetchData(url) };
                if (json.contains("quotes") && json["quotes"].isArray()) {
                    const auto& quotes = json["quotes"].getArray();
                    if (!quotes.empty()) {
                        const auto& quote = quotes[0];
                        if (quote.isObject()) {                            
                            Ticker ticker { quote["symbol"].getString() };
                            Company company { move(ticker) };
                            company.setName(quote["shortname"].getString());
                            company.setType(quote["quoteType"].getString());
                            company.setExchange(quote["exchDisp"].getString());
                            company.setSector(quote["sectorDisp"].getString());
                            company.setIndustry(quote["industryDisp"].getString());
                            
                            result = move(company);
                        }
                    }
                }

                return result;
            }
    };
}