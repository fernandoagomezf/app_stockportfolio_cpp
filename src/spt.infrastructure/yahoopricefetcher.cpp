export module spt.infrastructure:yahoopricefetcher;

import std;
import spt.domain;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;
import :restservice;

namespace spt::infrastructure::services {
    using std::chrono::seconds;
    using std::chrono::system_clock;
    using std::format;
    using std::string;
    using spt::domain::investments::Company;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::PriceFetcher;
    using spt::domain::investments::Price;
    using spt::domain::investments::Money;
    using spt::infrastructure::text::JsonValue;
    using spt::infrastructure::services::RestService;

    export class YahooPriceFetcher final : public RestService, public PriceFetcher {
        private:
            string _url;
            string _interval;
            string _range;

        public:        
            YahooPriceFetcher()
                : RestService(),
                  _url { "https://query1.finance.yahoo.com/v8/finance/chart" },
                  _interval { "1m" },
                  _range { "1d" }
            {
            }

            string getInterval() const {
                return _interval;
            }

            void setInterval(string interval) {
                _interval = interval;
            }

            string getRange() const {
                return _range;
            }

            void setRange(string range) {
                _range = range;
            }

            void fetch(Portfolio& portfolio) override {
                for (auto& ticker : portfolio.tickers()) {
                    Company& company { portfolio.getCompany(ticker) };
                    fetch(company);
                }
            }

            void fetch(Company& company) override {
                string url { 
                    format("{0}/{1}?range={2}&interval={3}",
                        _url, 
                        company.ticker().symbol(),
                        _range,
                        _interval
                    )
                };

                JsonValue json { fetchData(url) };
                const auto& result = json["chart"]["result"][0];
                const auto& timestamps = result["timestamp"].getArray();
                const auto& prices = result["indicators"]["quote"][0]["close"].getArray();
                
                for (size_t i = 0; i < prices.size() && i < timestamps.size(); i++) {
                    if (prices[i].isNumber() && timestamps[i].isNumber()) {
                        auto timestamp = system_clock::from_time_t(static_cast<time_t>(timestamps[i].getNumber()));
                        company.updatePrice(
                            timestamp,
                            Price { Money { prices[i].getNumber() } }
                        );
                    }
                }
            }
    };
}
