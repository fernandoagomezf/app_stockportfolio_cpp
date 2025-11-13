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
            string _exchange;
            string _name;
            string _type;
            string _sector;
            string _industry;
            vector<Transaction> _transactions;
            stack<PricePoint> _pricePoints;

        public:
            explicit Company(Ticker ticker)
                : _ticker { ticker },
                  _name { ticker.symbol() },
                  _type { },
                  _transactions { },
                  _pricePoints { },
                  _exchange { },
                  _sector { }
            {
            }

            Ticker ticker() const {
                return _ticker;
            }

            string getExchange() const {
                return _exchange;
            }

            void setExchange(const string& exchange) {
                _exchange = exchange;
            }

            string getName() const {
                return _name;
            }

            void setName(const string& name) {
                if (name.empty()) {
                    _name = _ticker.symbol();
                } else {
                    _name = name;
                }
            }

            string getType() const {
                return _type;
            }

            void setType(const string& type) {
                _type = type;
            }

            string getSector() const {
                return _sector;
            }

            void setSector(const string& sector) {
                _sector = sector;
            }

            string getIndustry() const {
                return _industry;
            }

            void setIndustry(const string& industry) {
                _industry = industry;
            }

            Price currentPrice() const {
                if (_pricePoints.empty()) {
                    return Price::unknown();
                } else {
                    return _pricePoints
                        .top()
                        .price();
                }
            }

            Price priceFor(int shares) const {
                Price price { currentPrice() };
                Money amount { price.amount() };
                return Price { amount * shares };
            }

            void updatePrice(Price newPrice) {
                _pricePoints.emplace(newPrice);
            }

            vector<PricePoint> priceHistory() const {
                vector<PricePoint> history;
                stack<PricePoint> temp = _pricePoints;
                while (!temp.empty()) {
                    history.push_back(temp.top());
                    temp.pop();
                }
                // Reverse to get chronological order (oldest first)
                std::ranges::reverse(history);
                return history;
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

                _transactions.emplace_back(TransactionType::Buy, shares, currentPrice());
            }

            void sellShares(int shares) {
                if (shares <= 0) {
                    throw invalid_argument { "Number of shares to sell must be positive." };
                }
                if (shares > shareCount()) {
                    throw invalid_argument { "Cannot sell more shares than currently owned." };
                }

                _transactions.emplace_back(TransactionType::Sell, shares, currentPrice());
            }
    };
}