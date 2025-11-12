export module spt.domain:pricefetcher;

import std;
import :ticker;
import :company;
import :portfolio;

namespace spt::domain::investments {
    using std::string;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Company;
    using spt::domain::investments::Portfolio;

    export class PriceFetcher  {
        public:
            virtual ~PriceFetcher() = default;

            virtual void fetch(Company& company) = 0;
            virtual void fetch(Portfolio& portfolio) = 0;
    };
}