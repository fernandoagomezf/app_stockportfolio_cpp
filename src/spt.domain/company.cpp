export module spt.domain:company;

import std;
import :ticker;
import :transaction;
import :pricepoint;

namespace spt::domain::investments {
    using std::invalid_argument;
    using std::plus;
    using std::ranges::fold_left;
    using std::stack;
    using std::string;
    using std::vector;
    using std::views::filter;
    using std::views::transform;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Transaction;
    using spt::domain::investments::PricePoint;

    export class Company final {
        private:
            Ticker _ticker;
            string _name;
            vector<Transaction> _transactions;
            stack<PricePoint> _pricePoints;

        public:
            explicit Company(Ticker ticker)
                : _ticker { ticker },
                  _name { ticker.symbol() },
                  _transactions { },
                  _pricePoints { }
            {
            }

            Ticker ticker() const {
                return _ticker;
            }

            void updatePrice(Price newPrice) {
                _pricePoints.emplace(newPrice);
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

            void buyShares(int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to buy must be positive." };
                }

                PricePoint pt { _pricePoints.top() };
                _transactions.emplace_back(TransactionType::Buy, shares, pt.price());
            }

            void sellShares(int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to sell must be positive." };
                }
                if (shares > shareCount()) {
                    throw invalid_argument { "Cannot sell more shares than currently owned." };
                }

                PricePoint pt { _pricePoints.top() };
                _transactions.emplace_back(TransactionType::Sell, shares, pt.price());
            }
    };
}