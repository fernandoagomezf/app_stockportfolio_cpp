export module spt.domain:portfolio;

import std;
import :ticker;
import :company;
import :money;
import :companysearch;

namespace spt::domain::investments {
    using std::invalid_argument;
    using std::map;
    using std::move;
    using std::runtime_error;
    using std::string;
    using std::unique_ptr;
    using std::views::keys;
    using std::views::values;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Company;
    using spt::domain::investments::Money;

    export class Portfolio final {
        private:
            map<Ticker, Company> _companies;

        public:
            Portfolio()
                : _companies { }
            {
            }

            auto tickers() const {
                return _companies | keys;
            }

            Company& track(Ticker ticker) {
                if (!_companies.contains(ticker)) {
                    Company company { ticker };
                    _companies.emplace(ticker, move(company));                    
                }

                return _companies.at(ticker);
            }

            Company& track(Company company) {
                Ticker ticker = company.ticker();
                if (!_companies.contains(ticker)) {
                    _companies.emplace(ticker, move(company));                    
                }

                return _companies.at(ticker);
            }

            void untrack(Ticker ticker) {
                if (_companies.contains(ticker)) {
                    _companies.erase(ticker);
                }
            }

            void untrack(Company company) {
                untrack(company.ticker());
            }

            Company& getCompany(Ticker ticker) {
                if (!_companies.contains(ticker)) {
                    throw runtime_error {
                        "Company with ticker " + ticker.symbol() + " is not being tracked."
                    };
                }

                return _companies.at(ticker);
            }

            const Company& getCompany(Ticker ticker) const {
                if (!_companies.contains(ticker)) {
                    throw runtime_error {
                        "Company with ticker " + ticker.symbol() + " is not being tracked."
                    };
                }

                return _companies.at(ticker);
            }
            
            void updatePrice(Ticker ticker, Price newPrice) {
                Company& company { track(ticker) };
                company.updatePrice(newPrice);
            }
    };
}