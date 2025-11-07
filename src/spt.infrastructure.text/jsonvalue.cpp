module spt.infrastructure.text:jsonvalue;

import std;
import :jsonvalue;

using std::get;
using std::holds_alternative;
using std::monostate;
using std::out_of_range;
using std::runtime_error;
using std::invalid_argument;
using std::string;
using spt::infrastructure::text::JsonValue;

JsonValue::JsonValue()
    : _value{ json_null{} }
{
}

JsonValue::JsonValue(json_null value)
    : _value{ value }
{
}

JsonValue::JsonValue(json_boolean value)
    : _value{ value }
{
}

JsonValue::JsonValue(json_number value)
    : _value{ value }
{
}

JsonValue::JsonValue(json_string value)
    : _value{ value }
{
}

JsonValue::JsonValue(json_array value)
    : _value{ value }
{
}

JsonValue::JsonValue(json_object value)
    : _value{ value }
{
}

bool JsonValue::isNull() const {
    return holds_alternative<json_null>(_value);
}

bool JsonValue::isBoolean() const {
    return holds_alternative<json_boolean>(_value);
}

bool JsonValue::isNumber() const {
    return holds_alternative<json_number>(_value);
}

bool JsonValue::isString() const {
    return holds_alternative<json_string>(_value);
}

bool JsonValue::isArray() const {
    return holds_alternative<json_array>(_value);
}

bool JsonValue::isObject() const {
    return holds_alternative<json_object>(_value);
}

const JsonValue::json_boolean& JsonValue::getBoolean() const {
    return get<JsonValue::json_boolean>(_value);
}

const JsonValue::json_number& JsonValue::getNumber() const {
    return get<JsonValue::json_number>(_value);
}

const JsonValue::json_string& JsonValue::getString() const {
    return get<JsonValue::json_string>(_value);
}

const JsonValue::json_array& JsonValue::getArray() const {
    return get<JsonValue::json_array>(_value);
}

const JsonValue::json_object& JsonValue::getObject() const {
    return get<JsonValue::json_object>(_value);
}

size_t JsonValue::count() const {
    size_t result { 0 };
    if (isArray()) {
        return getArray().size();
    } else if (isObject()) {
        return getObject().size();
    }
    return result;
}

const JsonValue& JsonValue::operator[](size_t index) const {
    if (!isArray()) {
        throw out_of_range{"Indexing operator[] called on non-array JsonValue"};
    }
    return getArray().at(index);
}

bool JsonValue::contains(const string& key) const {
    if (!isObject()) {
        return false;
    }
    return getObject().contains(key);
}

const JsonValue& JsonValue::operator[](const string& key) const {
    if (!isObject()) {
        throw out_of_range{"Indexing operator[] called on non-object JsonValue"};
    }
    return getObject().at(key);
}

