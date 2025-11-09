export module spt.domain:pricepoint;

import std;
import :price;

namespace spt::domain::investments {
    using std::chrono::system_clock;
    using std::invalid_argument;
    using spt::domain::investments::Price;

    export class PricePoint final {
        private:
            system_clock::time_point _stamp;
            Price _price;

        public:            
            explicit PricePoint(Price price)
                : _stamp { system_clock::now() }, 
                  _price { price } 
            {
            }

            PricePoint(system_clock::time_point stamp, Price price)
                : _stamp { stamp }, 
                  _price { price } 
            {
            }

            system_clock::time_point stamp() const {
                return _stamp;
            }

            Price price() const {
                return _price;
            }

            int compareTime(const PricePoint& other) const {
                if (_stamp < other._stamp) {
                    return -1;
                } else if (_stamp > other._stamp) {
                    return 1;
                } else {
                    return 0;
                }
            }

            int comparePrice(const PricePoint& other) const {
                if (_price < other._price) {
                    return -1;
                } else if (_price > other._price) {
                    return 1;
                } else {
                    return 0;
                }
            }
    };
}