module spt.infrastructure.sql:value;

import std;
import :value;

using std::get;
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

Value::Value(long value) 
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

int Value::getInt() const {
    return get<int>(_value);
}

int Value::getLong() const {
    return get<long>(_value);
}

int Value::getDouble() const {
    return get<double>(_value);
}

string Value::getString() const {
    return get<string>(_value);
}