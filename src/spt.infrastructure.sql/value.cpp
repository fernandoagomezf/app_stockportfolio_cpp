export module spt.infrastructure.sql:value;

import std;

namespace spt::infrastructure::sql {
    using std::byte;
    using std::monostate;
    using std::string;
    using std::variant;
    using std::vector;

    export class Value {
        public:
            using blob = vector<byte>;

            Value() 
                : _value(monostate{}) {
            }

            Value(int value) 
                : _value(value) {
            }

            Value(long long value) 
                : _value(value) {
            }

            Value(double value) 
                : _value(value) {
            }

            Value(string value) 
                : _value(value) {
            }

            Value(blob value)   
                : _value(value) {
            }

            bool isNull() const {
                return holds_alternative<monostate>(_value);
            }

            bool isInt() const {
                return holds_alternative<int>(_value);
            }

            bool isLong() const {
                return holds_alternative<long long>(_value);
            }

            bool isDouble() const {
                return holds_alternative<double>(_value);
            }

            bool isString() const {
                return holds_alternative<string>(_value);
            }

            bool isBlob() const {
                return holds_alternative<blob>(_value);
            }

            int getInt() const {
                return get<int>(_value);
            }

            long long getLong() const {
                return get<long long>(_value);
            }

            double getDouble() const {
                return get<double>(_value);
            }

            const string& getString() const {
                return get<string>(_value);
            }

            const blob& getBlob() const {
                return get<blob>(_value);
            }
            
        private:
            variant<monostate, int, long long, double, string, blob> _value;
    };
}