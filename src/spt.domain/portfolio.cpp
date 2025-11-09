export module spt.domain:portfolio;

import std;
import :ticker;
import :company;

namespace spt::domain::investments {
    using std::map;
    using std::move;
    using std::string;

    export class Portfolio final {
        private:
            map<Ticker, Company> _companies;

        public:
            void track(const Ticker& ticker, const string& name) {
                Company company { ticker, name };
                _companies.emplace(ticker, move(company));
            }

            void track(const Ticker& ticker) {
                Company company { ticker, ticker };
                _companies.emplace(ticker, move(company));
            }

            
    };
}