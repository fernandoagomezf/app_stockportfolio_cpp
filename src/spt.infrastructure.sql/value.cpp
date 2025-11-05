module spt.infrastructure.sql:value;

import std;
import :value;

using std::get;
using std::holds_alternative;
using std::monostate;
using std::string;
using spt::infrastructure::sql::Value;

Value::Value()
    : _value(monostate{}) 
{
}

Value::Value(int value)
    : _value(value)
{
}

Value::Value(long long value) 
    : _value(value) 
{
}

Value::Value(double value) 
    : _value(value)
{
}

Value::Value(string value) 
    : _value(value)
{
}

Value::Value(Value::blob value)
    : _value(value)
{
}

bool Value::isNull() const {
    return holds_alternative<monostate>(_value);
}

bool Value::isInt() const {
    return holds_alternative<int>(_value);
}

bool Value::isLong() const {
    return holds_alternative<long long>(_value);
}

bool Value::isDouble() const {
    return holds_alternative<double>(_value);
}

bool Value::isString() const {
    return holds_alternative<string>(_value);
}

bool Value::isBlob() const {
    return holds_alternative<blob>(_value);
}

int Value::getInt() const {
    return get<int>(_value);
}

long long Value::getLong() const {
    return get<long long>(_value);
}

double Value::getDouble() const {
    return get<double>(_value);
}

string Value::getString() const {
    return get<string>(_value);
}

Value::blob Value::getBlob() const {
    return get<blob>(_value);
}
