export module spt.domain:company;

import std;
import :transaction;
import :pricepoint;

namespace spt::domain::investments {
    using std::invalid_argument;
    using std::plus;
    using std::ranges::fold_left;
    using std::string;
    using std::vector;
    using std::views::filter;
    using std::views::transform;
    using spt::domain::investments::Transaction;
    using spt::domain::investments::PricePoint;

    export class Company final {
        private:
            string _symbol;
            string _name;
            vector<Transaction> _transactions;
            vector<PricePoint> _pricePoints;

        public:
            Company(const string& symbol, const string& name)
                : _symbol { symbol },
                  _name { name },
                  _transactions { },
                  _pricePoints { }
            {
                if (symbol.empty()) {
                    throw invalid_argument { "The company ticker symbol cannot be empty" };
                }
                if (name.empty()) {
                    throw invalid_argument { "The company must have a valid name" };
                }
            }

            void changePricePoint(double newPrice) {
                if (newPrice < 0.01) {
                    throw invalid_argument { "Price cannot be less than one cent" };
                }
            }

            int sharesBought() const {
                auto bought = _transactions
                    | filter([](const auto& tx) { return tx.isBuying(); })
                    | transform([](const auto& tx) { return tx.getShares(); });
                auto result = fold_left(bought, 0, plus<>{});
                return result;
            }

            int sharesSold() const {
                auto sold = _transactions
                    | filter([](const auto& tx) { return tx.isSelling(); })
                    | transform([](const auto& tx) { return tx.getShares(); });
                auto result = fold_left(sold, 0, plus<>{});
                return result;
            }

            int shareCount() const {
                auto total = _transactions
                    | transform([] (const auto& tx) { return tx.isBuying() ? tx.getShares() : -tx.getShares(); });
                auto result = fold_left(total, 0, plus<>{});
                return result;
            }
    };
}