export module spt.infrastructure:jsonvalue;

import std;

namespace spt::infrastructure::text {
    using std::map;
    using std::monostate;
    using std::out_of_range;
    using std::string;
    using std::variant;
    using std::vector;

    export class JsonValue {
        public:
            using json_null = monostate;
            using json_boolean = bool;
            using json_number = double;
            using json_string = string;
            using json_array = vector<JsonValue>;
            using json_object = map<string, JsonValue>;

            JsonValue()
                : _value{ json_null{} } {
            }

            JsonValue(json_null value)
                : _value{ value } {
            }

            JsonValue(json_boolean value)
                : _value{ value } {
            }

            JsonValue(json_number value)
                : _value{ value } {
            }

            JsonValue(json_string value)
                : _value{ value } {
            }

            JsonValue(json_array value)
                : _value{ value } {
            }

            JsonValue(json_object value)
                : _value{ value } {
            }

            bool isNull() const{
                return holds_alternative<json_null>(_value);
            }

            bool isBoolean() const {
                return holds_alternative<json_boolean>(_value);
            }

            bool isNumber() const {
                return holds_alternative<json_number>(_value);
            }

            bool isString() const {
                return holds_alternative<json_string>(_value);
            }

            bool isArray() const {
                return holds_alternative<json_array>(_value);
            }

            bool isObject() const {
                return holds_alternative<json_object>(_value);
            }

            const json_boolean& getBoolean() const {
                return get<json_boolean>(_value);
            }

            const json_number& getNumber() const {
                return get<json_number>(_value);
            }

            const json_string& getString() const {
                return get<json_string>(_value);
            }

            const json_array& getArray() const {
                return get<json_array>(_value);
            }

            const json_object& getObject() const {
                return get<json_object>(_value);
            }
            
            size_t count() const {
                size_t result { 0 };
                if (isArray()) {
                    return getArray().size();
                } else if (isObject()) {
                    return getObject().size();
                }
                return result;
            }

            bool contains(const string& key) const {
                if (!isObject()) {
                    return false;
                }
                return getObject().contains(key);
            }

            const JsonValue& operator[](size_t index) const {
                if (!isArray()) {
                    throw out_of_range{"Indexing operator[] called on non-array JsonValue"};
                }
                return getArray().at(index);
            }

            const JsonValue& operator[](const string& key) const {
                if (!isObject()) {
                    throw out_of_range{"Indexing operator[] called on non-object JsonValue"};
                }                    
                return getObject().at(key);
            }

            bool operator==(const JsonValue& other) const = default;

        private:
            variant<json_null, json_boolean, json_number, json_string, json_array, json_object> _value;
    };
}
