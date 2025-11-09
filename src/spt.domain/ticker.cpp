export module spt.domain:ticker;

import std;

namespace spt::domain::investments {
    using std::invalid_argument;
    using std::string;
    using std::string_view;

    export class Ticker final {
        private:
            string _symbol;

        public:
            explicit Ticker(string_view symbol)
                : _symbol { symbol }
            {
                if (symbol.empty()) {
                    throw invalid_argument { "The company ticker symbol cannot be empty" };
                }
            }

            string symbol() const {
                return _symbol;
            }

            auto operator<=>(const Ticker& other) const = default;
    };
}