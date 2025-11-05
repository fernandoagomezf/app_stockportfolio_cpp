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

            Value();
            Value(int value);
            Value(long long value);
            Value(double value);
            Value(string value);
            Value(blob value);

            bool isNull() const;
            bool isInt() const;
            bool isLong() const;
            bool isDouble() const;
            bool isString() const;
            bool isBlob() const;

            int getInt() const;
            long long getLong() const;
            double getDouble() const;
            string getString() const;
            blob getBlob() const;
            
        private:
            variant<monostate, int, long long, double, string, blob> _value;
    };
}