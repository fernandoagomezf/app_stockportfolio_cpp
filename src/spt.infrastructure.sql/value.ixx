export module spt.infrastructure.sql:value;

import std;

namespace spt::infrastructure::sql {
    using std::monostate;
    using std::string;
    using std::variant;

    export class Value {
        public:
            Value();
            Value(int value);
            Value(long long v);
            Value(double v);
            Value(string v);

            bool isNull() const;
            bool isInt() const;
            bool isLong() const;
            bool isDouble() const;
            bool isString() const;

            int getInt() const;
            long long getLong() const;
            double getDouble() const;
            string getString() const;
            
        private:
            variant<monostate, int, long long, double, string> _value;
    };
}