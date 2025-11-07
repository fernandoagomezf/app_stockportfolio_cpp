export module spt.infrastructure.text:jsonparser;

import std;
import :jsonvalue;

namespace spt::infrastructure::text {
    using std::string;
    using std::string_view;

    export class JsonParser {
        public:
            static JsonValue parse(string_view json);

        private:
            JsonParser(string_view json);

            JsonValue parseValue();
            JsonValue parseObject();
            JsonValue parseArray();
            JsonValue parseString();
            JsonValue parseNumber();
            JsonValue parseTrue();
            JsonValue parseFalse();
            JsonValue parseNull();

            void skipWhitespace();
            char peek() const;
            char consume();
            bool eof() const;
            void expect(char ch);

            string_view _json;
            size_t _position;
            size_t _line;
            size_t _column;
    };
}
