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
    using spt::domain::investments::Company;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::PriceFetcher;
    using spt::infrastructure::services::RestService;

    export class AlphaVantageFetcher final : public RestService, public PriceFetcher {
        public:

            void fetch(Company& company) override {
                // Implement fetching logic here
            }

            void fetch(Portfolio& portfolio) override {
                // Implement fetching logic here
            }
    };
}
