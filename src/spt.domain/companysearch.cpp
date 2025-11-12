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

            virtual optional<Company> search(Ticker ticker) = 0;
            virtual optional<Company> search(string name) = 0;
    };
}