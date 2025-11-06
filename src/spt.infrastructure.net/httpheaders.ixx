export module spt.infrastructure.net:httpheaders;

import std;

namespace spt::infrastructure::net {
    using std::map;
    using std::optional;
    using std::string;
    using std::string_view;

    export class HttpHeaders final {
        public:
            using iterator = map<string, string>::iterator;
            using const_iterator = map<string, string>::const_iterator;

            bool contains(const string& key) const;
            optional<string> get(const string& key) const;
            void set(const string& key, const string& value);

            iterator begin();
            iterator end();
            const_iterator begin() const;
            const_iterator end() const;

        private:
            map<string, string> _headers;
    };
}