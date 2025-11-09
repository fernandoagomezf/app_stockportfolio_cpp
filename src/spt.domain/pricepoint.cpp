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

            Price getPrice() const {
                return _price;
            }

            system_clock::time_point getStamp() const {
                return _stamp;
            }
    };
}