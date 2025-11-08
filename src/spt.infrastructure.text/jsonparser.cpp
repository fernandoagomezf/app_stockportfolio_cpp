export module spt.infrastructure.text:jsonparser;

import std;
import :jsonvalue;

namespace spt::infrastructure::text {
    using std::format;
    using std::isdigit;
    using std::isspace;
    using std::runtime_error;
    using std::string;
    using std::string_view;

    export class JsonParser {
        private:        
            string_view _json;
            size_t _position;
            size_t _line;
            size_t _column;

            JsonParser(string_view json)
                : _json { json }, 
                  _position { 0 }, 
                  _line { 1 }, 
                  _column { 1 } {
            }

            JsonValue parseValue() {
                skipWhitespace();
                if (eof()) {
                    throw runtime_error {
                        format("Unexpected end of input ({}, {})", _line, _column)
                    };
                }

                auto ch { peek() };
                switch (ch) {
                    case '{':
                        return parseObject();
                    case '[':
                        return parseArray();
                    case '"':
                        return parseString();
                    case 't':
                        return parseTrue();
                    case 'f':
                        return parseFalse();
                    case 'n':
                        return parseNull();
                    case '-':
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        return parseNumber();
                    default:
                        throw runtime_error {
                            format("Unexpected character '{0}' ({1}:{2})", ch, _line, _column)
                        };
                }
            }

            JsonValue parseObject() {
                expect('{');

                JsonValue::json_object object { };
                
                skipWhitespace();
                if (peek() == '}') { // empty json object
                    consume();
                    return JsonValue { object };
                }

                while (true) {
                    skipWhitespace();

                    if (peek() != '"') {
                        throw runtime_error {
                            format("Expected string key in object ({0}:{1})", _line, _column)
                        };
                    }

                    auto keyResult { parseString() };
                    string key = keyResult.getString();
                    skipWhitespace();
                    expect(':');
                    auto valueResult { parseValue() };
                    object[key] = valueResult;
                    skipWhitespace();

                    auto next { peek() };
                    if (next == '}') {
                        consume();
                        break;
                    } else if (next == ',') {
                        consume();
                        continue;
                    } else {
                        throw runtime_error {
                            format("Expected ',' or '}}' in object ({0}:{1})", _line, _column)
                        };
                    }
                }

                return JsonValue { object };
            }

            JsonValue parseArray() {
                expect('[');
    
                JsonValue::json_array array { };
                
                skipWhitespace();
                if (peek() == ']') {    // empty json array
                    consume();
                    return JsonValue{ array };
                }

                while (true) {
                    auto valueResult = parseValue();
                    array.push_back(valueResult);
                    skipWhitespace();

                    auto next { peek() };
                    if (next == ']') {
                        consume();
                        break;
                    } else if (next == ',') {
                        consume();
                        continue;
                    } else {
                        throw runtime_error {
                            format("Expected ',' or ']' in array ({0}:{1})", _line, _column)
                        };
                    }
                }

                return JsonValue { array };
            }

            JsonValue parseString(){
                string result { };
    
                expect('"');
                while (!eof() && peek() != '"') {
                    auto ch { consume() };
                    if (ch == '\\') {
                        if (eof()) {
                            throw runtime_error {
                                format("Unterminated string escape ({0}:{1})", _line, _column)
                            };
                        }

                        auto escaped { consume() };
                        switch (escaped) {
                            case '"':  result += '"'; break;
                            case '\\': result += '\\'; break;
                            case '/':  result += '/'; break;
                            case 'b':  result += '\b'; break;
                            case 'f':  result += '\f'; break;
                            case 'n':  result += '\n'; break;
                            case 'r':  result += '\r'; break;
                            case 't':  result += '\t'; break;
                            case 'u': { // simplified unicode escape
                                if (_position + 4 > _json.size()) {
                                    throw runtime_error {
                                        format("Invalid unicode escape ({0}:{1})", _line, _column)
                                    };
                                }
                                string hex { };
                                for (int i = 0; i < 4; ++i) {
                                    hex += consume();
                                }
                                result += "\\u" + hex;  // just append the hex sequence instead of converting to UTF-8
                                break;
                            }
                            default:
                                throw runtime_error {
                                    format("Invalid escape sequence '\\{0}' ({1}:{2})", escaped, _line, _column)
                                };
                        }
                    } else {
                        result += ch;
                    }
                }

                if (eof()) {
                    throw runtime_error {
                        format("Unterminated string ({0}:{1})", _line, _column)
                    };
                }
                expect('"');

                return JsonValue { result };
            }

            JsonValue parseNumber() {
                size_t start { _position };
    
                if (peek() == '-') {
                    consume();
                }
                if (eof() || !isdigit(peek())) {
                    throw runtime_error {
                        format("Invalid number format ({0}:{1})", _line, _column)
                    };
                }

                if (peek() == '0') {
                    consume();
                } else {
                    while (!eof() && isdigit(peek())) {
                        consume();
                    }
                }

                if (!eof() && peek() == '.') {
                    consume();
                    if (eof() || !isdigit(peek())) {
                        throw runtime_error {
                            format("Invalid number format: expected digit after '.' ({0}:{1})", _line, _column)
                        };
                    }
                    while (!eof() && isdigit(peek())) {
                        consume();
                    }
                }

                if (!eof() && (peek() == 'e' || peek() == 'E')) {
                    consume();
                    if (!eof() && (peek() == '+' || peek() == '-')) {
                        consume();
                    }
                    if (eof() || !isdigit(peek())) {
                        throw runtime_error {
                            format("Invalid number format: expected digit in exponent ({0}:{1})", _line, _column)
                        };
                    }
                    while (!eof() && isdigit(peek())) {
                        consume();
                    }
                }

                string numberStr {
                    _json.substr(start, _position - start)
                };
                double number { std::stod(numberStr) };

                return JsonValue { number };
            }

            JsonValue parseTrue() {
                if (_position + 4 > _json.size() || _json.substr(_position, 4) != "true") {
                    throw runtime_error {
                        format("Invalid literal: expected 'true' ({0}:{1})", _line, _column)
                    };
                }
                _position += 4;
                _column += 4;
                return JsonValue { true };
            }
            
            JsonValue parseFalse() {
                if (_position + 5 > _json.size() || _json.substr(_position, 5) != "false") {
                    throw runtime_error {
                        format("Invalid literal: expected 'false' ({0}:{1})", _line, _column)
                    };
                }
                _position += 5;
                _column += 5;
                return JsonValue { false };
            }
            
            JsonValue parseNull() {
                if (_position + 4 > _json.size() || _json.substr(_position, 4) != "null") {
                    throw runtime_error {
                        format("Invalid literal: expected 'null' ({0}:{1})", _line, _column)
                    };
                }
                _position += 4;
                _column += 4;
                return JsonValue{};
            }

            void skipWhitespace() {
                while (!eof()) {
                    auto ch  { peek() };
                    if (isspace(ch)) {
                        if (ch == '\n') {
                            ++_line;
                            _column = 1;
                        } else {
                            ++_column;
                        }
                        ++_position;
                    } else {
                        break;
                    }
                }
            }

            char peek() const {
                if (eof()) {
                    return '\0';
                }
                return _json[_position];
            }
            
            char consume() {
                if (eof()) {
                    return '\0';
                }
                auto ch { _json[_position++] };
                if (ch == '\n') {
                    ++_line;
                    _column = 1;
                } else {
                    ++_column;
                }
                return ch;
            }
            
            bool eof() const {
                return _position >= _json.size();
            }
            
            void expect(char ch) {
                if (eof()) {
                    throw runtime_error {
                        format("Expected '{0}' but reached end of input ({1}:{2})", ch, _line, _column)
                    };
                }
                auto peeked { peek() };
                if (peeked != ch) {
                    throw runtime_error {
                        format("Expected '{0}' but found '{1}' ({2}:{3})", ch, peeked, _line, _column)
                    };
                }
                consume();
            }

        public:
            static JsonValue parse(string_view json) {
                JsonParser parser { json };
                auto result = parser.parseValue();
                parser.skipWhitespace();
                if (!parser.eof()) {
                    throw runtime_error {
                        format("Unexpected JSON content: end of file was expected ({0}:{1})", parser._line, parser._column)
                    };
                }

                return result;
            }
    };
}
