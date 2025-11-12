export module spt.infrastructure:alphavantagefetcher;

import std;
import spt.domain;
import :httpclient;
import :httprequest;
import :httpresponse;
import :jsonvalue;
import :jsonparser;

namespace spt::infrastructure::services {
    using spt::domain::investments::Company;
    using spt::domain::investments::PriceFetcher;

    export class AlphaVantageFetcher final : public PriceFetcher {

    };
}
