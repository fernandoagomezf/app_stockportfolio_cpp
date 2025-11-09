export module spt.domain:money;

import std;

namespace spt::domain::investments {
    using std::abs;

    export class Money final {
        private:
            double _value;

        public:
            explicit Money(double value)
                : _value { value }
            {
            }
            
            static Money zero() {
                return Money { 0.0 };
            }

            double value() const {
                return _value;
            }
            
            bool isNegative() {
                return _value < -0.01;
            }

            bool isPositive() {
                return _value > 0.01;
            }

            bool isZero() const {
                return abs(_value) < 0.01;
            }

            auto operator<=>(const Money& other) const = default;
            
            auto operator*(int factor) const {
                return Money { _value * factor };
            }

            auto operator+ (const Money& other) const {
                return Money { _value + other._value };
            }

            auto operator+= (const Money& other) {
                _value += other._value;
                return *this;
            }

            auto operator- (const Money& other) const {
                return Money { _value - other._value };
            }

            auto operator-= (const Money& other) {
                _value -= other._value;
                return *this;
            }
    };
}