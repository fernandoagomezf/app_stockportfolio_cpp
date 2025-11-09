export module spt.domain:transaction;

import std;

namespace spt::domain::investments {
    using std::chrono::system_clock;
    using std::format;
    using std::invalid_argument;

    export enum class TransactionType {
        Buy, Sell
    };

    export class Transaction final {
        private:
            TransactionType _type;
            int _shares;
            double _unitPrice;
            system_clock::time_point _stamp;

        public:
            Transaction(TransactionType type, int shares, double unitPrice) 
                : _type { type },
                  _shares { shares },
                  _unitPrice { unitPrice },
                  _stamp { system_clock::now() }
            {
                if (shares <= 0) {
                    throw invalid_argument { "Share count cannot be zero or negative" };
                }
                if (unitPrice <= 0.001) {
                    throw invalid_argument { "Share unit price cannot be less than one cent" };
                }
            }

            TransactionType getType() const {
                return _type;
            }

            int getShares() const {
                return _shares;
            }

            double getUnitPrice() const {
                return _unitPrice;
            }

            double getTotalValue() const {
                return _shares * _unitPrice;
            }

            system_clock::time_point getStamp() const {
                return _stamp;
            }
    };
}
