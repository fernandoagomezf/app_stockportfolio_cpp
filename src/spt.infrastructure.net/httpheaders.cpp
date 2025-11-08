export module spt.infrastructure.net:httpheaders;

import std;

namespace spt::infrastructure::net {
    using std::map;
    using std::nullopt;
    using std::optional;
    using std::string;
    using std::string_view;

    export class HttpHeaders final {
        public:
            using iterator = map<string, string>::iterator;
            using const_iterator = map<string, string>::const_iterator;

            bool contains(const string& key) const {
                return _headers.contains(key);
            }

            optional<string> get(const string& key) const {
                optional<string> result { nullopt };
                auto it = _headers.find(key);
                if (it != _headers.end()) {
                    result = it->second;
                }

                return result;
            }

            void set(const string& key, const string& value) {
                _headers[key] = value;
            }

            iterator begin() {
                return _headers.begin();
            }

            iterator end() {
                return _headers.end();
            }

            const_iterator begin() const {
                return _headers.begin();
            }

            const_iterator end() const {
                return _headers.end();
            }

        private:
            map<string, string> _headers;
    };
}