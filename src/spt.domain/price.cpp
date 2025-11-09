export module spt.domain:price;

import std;
import :money;

namespace spt::domain::investments {
    using std::invalid_argument;
    using spt::domain::investments::Money;

    export class Price final {
        private:
            Money _amount;

        public:
            explicit Price(Money amount)
                : _amount { amount }
            {
                if (amount.isZero()) {
                    throw invalid_argument { "Price cannot be zero" };
                }
            }

            Money amount() const {
                return _amount;
            }

            auto operator<=>(const Price& other) const = default;
            
            auto operator*(int factor) const {
                return Price { _amount * factor };
            }
    };
}