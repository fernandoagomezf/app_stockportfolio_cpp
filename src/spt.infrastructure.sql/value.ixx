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
            Value(long v);
            Value(double v);
            Value(string v);

            int getInt() const;
            int getLong() const;
            int getDouble() const;
            string getString() const;
            
        private:
            variant<monostate, int, long, double, string> _value;
    };
}