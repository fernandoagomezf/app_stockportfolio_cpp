export module spt.domain:company;

import std;
import :ticker;
import :transaction;
import :pricepoint;
import :pricedelta;

namespace spt::domain::investments {
    using std::chrono::system_clock;
    using std::invalid_argument;
    using std::nullopt;
    using std::optional;
    using std::plus;
    using std::ranges::fold_left;
    using std::ranges::reverse;
    using std::stack;
    using std::string;
    using std::vector;
    using std::views::filter;
    using std::views::transform;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Transaction;
    using spt::domain::investments::PricePoint;
    using spt::domain::investments::PriceDelta;

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

            void updatePrice(system_clock::time_point timestamp, Price newPrice) {
                _pricePoints.emplace(timestamp, newPrice);
            }

            system_clock::time_point latestPriceTimestamp() const {
                if (_pricePoints.empty()) {
                    return system_clock::time_point{}; // Return epoch if no prices
                }
                return _pricePoints.top().stamp();
            }

            void clearPriceHistory() {
                while (!_pricePoints.empty()) {
                    _pricePoints.pop();
                }
            }

            vector<PricePoint> priceHistory() const {
                vector<PricePoint> history;
                stack<PricePoint> temp = _pricePoints;
                while (!temp.empty()) {
                    history.push_back(temp.top());
                    temp.pop();
                }
                
                reverse(history);
                return history;
            }

            optional<PriceDelta> delta() const {
                optional<PriceDelta> result { nullopt };

                if (_pricePoints.size() >= 2) {
                    PricePoint after = _pricePoints.top();  
                    
                    stack<PricePoint> temp = _pricePoints;
                    while (temp.size() > 1) {
                        temp.pop();
                    }
                    PricePoint before = temp.top();

                    result = PriceDelta { before, after };
                }

                return result;
            }

            optional<Price> minPrice() const {
                if (_pricePoints.empty()) return nullopt;

                stack<PricePoint> temp = _pricePoints;
                Price minVal = temp.top().price();

                while (!temp.empty()) {
                    if (temp.top().price().amount().value() < minVal.amount().value()) {
                        minVal = temp.top().price();
                    }
                    temp.pop();
                }

                return minVal;
            }

            optional<Price> maxPrice() const {
                if (_pricePoints.empty()) return nullopt;

                stack<PricePoint> temp = _pricePoints;
                Price maxVal = temp.top().price();

                while (!temp.empty()) {
                    if (temp.top().price().amount().value() > maxVal.amount().value()) {
                        maxVal = temp.top().price();
                    }
                    temp.pop();
                }

                return maxVal;
            }
    };
}