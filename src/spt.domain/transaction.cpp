export module spt.domain:transaction;

import std;
import :price;

namespace spt::domain::investments {
    using std::chrono::system_clock;
    using std::format;
    using std::invalid_argument;
    using spt::domain::investments::Price;

    export enum class TransactionType {
        Buy, Sell
    };

    export class Transaction final {
        private:
            TransactionType _type;
            int _shares;
            Price _unitPrice;
            system_clock::time_point _stamp;

        public:
            Transaction(TransactionType type, int shares, Price unitPrice) 
                : _type { type },
                  _shares { shares },
                  _unitPrice { unitPrice },
                  _stamp { system_clock::now() }
            {
                if (shares <= 0) {
                    throw invalid_argument { "Share count cannot be zero or negative" };
                }
            }

            TransactionType getType() const {
                return _type;
            }

            bool isBuying() const {
                return _type == TransactionType::Buy;
            }

            bool isSelling() const {
                return _type == TransactionType::Sell;
            }

            int getShares() const {
                return _shares;
            }

            Price getUnitPrice() const {
                return _unitPrice;
            }

            Price getTotalValue() const {
                return _unitPrice * _shares;
            }

            system_clock::time_point getStamp() const {
                return _stamp;
            }
    };
}
