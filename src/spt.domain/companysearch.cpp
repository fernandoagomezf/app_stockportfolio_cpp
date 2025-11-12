export module spt.domain:companysearch;

import std;
import :ticker;
import :company;

namespace spt::domain::investments {
    using std::optional;
    using std::string;
    using spt::domain::investments::Ticker;

    export class CompanySearch  {
        public:
            virtual ~CompanySearch() = default;

            virtual optional<Company> searchBySymbol(Ticker ticker) = 0;
            virtual optional<Company> searchByName(string name) = 0;
    };
}