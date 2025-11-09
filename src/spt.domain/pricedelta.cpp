export module spt.domain:pricedelta;

import std;
import :pricepoint;

namespace spt::domain::investments {
    using std::invalid_argument;    
    using spt::domain::investments::PricePoint;

    export class PriceDelta final {
        private:
            PricePoint _before;
            PricePoint _after;

        public:
            explicit PriceDelta(PricePoint before, PricePoint after)
                : _before { before },
                  _after { after }
            {
                if (before.compareTime(after) >= 0) {
                    throw invalid_argument { "The 'before' price point must be earlier than the 'after' price point" };
                }
            }

            PricePoint before() const {
                return _before;
            }

            PricePoint after() const {
                return _after;
            }

            Money priceDelta() const {
                return _after.price() - _before.price();
            }
    };
}