export module spt.domain:ticker;

import std;

namespace spt::domain::investments {
    using std::string;
    using std::string_view;

    export class Ticker final {
        private:
            string _symbol;

        public:
            explicit Ticker(string_view symbol)
                : _symbol { symbol }
            {
            }

            auto operator<=>(const Ticker& other) const = default;
    };
}