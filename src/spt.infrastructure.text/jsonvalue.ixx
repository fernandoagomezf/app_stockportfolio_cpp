export module spt.infrastructure.text:jsonvalue;

import std;

namespace spt::infrastructure::text {
    using std::map;
    using std::monostate;
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

            JsonValue();
            JsonValue(json_null value);
            JsonValue(json_boolean value);  
            JsonValue(json_number value);
            JsonValue(json_string value);
            JsonValue(json_array value);
            JsonValue(json_object value);

            bool isNull() const;
            bool isBoolean() const;
            bool isNumber() const;
            bool isString() const;
            bool isArray() const;
            bool isObject() const;

            const json_boolean& getBoolean() const;
            const json_number& getNumber() const;
            const json_string& getString() const;
            const json_array& getArray() const;
            const json_object& getObject() const;
            
            size_t count() const;
            bool contains(const string& key) const;
            const JsonValue& operator[](size_t index) const;
            const JsonValue& operator[](const string& key) const;

            bool operator==(const JsonValue& other) const = default;

        private:
            variant<json_null, json_boolean, json_number, json_string, json_array, json_object> _value;
    };
}
