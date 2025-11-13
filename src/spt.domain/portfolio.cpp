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
            Money _capital { 0.0 };
            map<Ticker, Company> _companies;

        public:
            Portfolio()
                : _capital { 0.0 },
                  _companies { }
            {
            }

            auto companies() const {
                return _companies | values;
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

            Money capital() const {
                return _capital;
            }

            void capitalize(Money amount) {
                if (!amount.isPositive()) {
                    throw invalid_argument {
                        "Capital amount must be positive"
                    };
                }
                _capital += amount;
            }

            void buyShares(Ticker ticker, int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to buy must be positive." };
                }

                Company& company { track(ticker) };
                Money cost { company.priceFor(shares) };
                if (cost > _capital) {
                    throw invalid_argument { "Insufficient capital to complete purchase." };
                }

                company.buyShares(shares);
            }

            void sellShares(Ticker ticker, int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to sell must be positive." };
                }

                Company& company { track(ticker) };
                company.sellShares(shares);
            }

            void updatePrice(Ticker ticker, Price newPrice) {
                Company& company { track(ticker) };
                company.updatePrice(newPrice);
            }
    };
}