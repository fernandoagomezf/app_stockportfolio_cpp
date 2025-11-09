export module spt.domain:pricepoint;

import std;

namespace spt::domain::investments {
    using std::chrono::system_clock;
    using std::invalid_argument;

    export class PricePoint final {
        private:
            system_clock::time_point _stamp;
            double _price;

        public:            
            explicit PricePoint(double price)
                : _stamp { system_clock::now() }, 
                  _price { price } 
            {
                if (price < 0.01) {
                    throw invalid_argument { "Price cannot be less than one cent" };
                }
            }

            PricePoint(system_clock::time_point stamp, double price)
                : _stamp { stamp }, 
                  _price { price } 
            {
                if (price < 0.01) {
                    throw invalid_argument { "Price cannot be less than one cent" };
                }
            }

            double getPrice() const {
                return _price;
            }

            system_clock::time_point getStamp() const {
                return _stamp;
            }
    };
}