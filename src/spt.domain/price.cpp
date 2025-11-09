export module spt.domain:price;

import std;

namespace spt::domain::investments {
    using std::invalid_argument;

    export class Price final {
        private:
            double _amount;

        public:
            explicit Price(double amount)
                : _amount { amount }
            {
                if (amount < 0.01) {
                    throw invalid_argument { "Price cannot be less than one cent" };
                }
            }

            auto operator<=>(const Price& other) const = default;
    };
}