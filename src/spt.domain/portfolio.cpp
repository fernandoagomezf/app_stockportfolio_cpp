export module spt.domain:portfolio;

import std;
import :ticker;
import :company;
import :money;

namespace spt::domain::investments {
    using std::invalid_argument;
    using std::map;
    using std::move;
    using std::string;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Company;
    using spt::domain::investments::Money;

    export class Portfolio final {
        private:
            Money _capital { 0.0 };
            map<Ticker, Company> _companies;

        public:
            void track(Ticker ticker) {
                if (!_companies.contains(ticker)) {
                    Company company { ticker };
                    _companies.emplace(ticker, move(company));                    
                }
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

                track(ticker);                
                Company company { _companies.at(ticker) };
                Money cost = company.priceFor(shares);
                if (cost > _capital) {
                    throw invalid_argument { "Insufficient capital to complete purchase." };
                }

                company.buyShares(shares);
            }

            void sellShares(Ticker ticker, int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to sell must be positive." };
                }

                track(ticker);
                Company company { _companies.at(ticker) };
                company.sellShares(shares);
            }

            void updatePrice(Ticker ticker, Price newPrice) {
                track(ticker);
                Company& company = _companies.at(ticker);
                company.updatePrice(newPrice);
            }
    };
}