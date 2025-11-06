export module spt.infrastructure.net:httpheaders;

import std;

namespace spt::infrastructure::net {
    using std::map;
    using std::string;
    using std::string_view;

    export class HttpHeaders final {
        public:
            using iterator = map<string, string>::iterator;
            using const_iterator = map<string, string>::const_iterator;

            bool contains(string_view key) const;
            string get(string_view key) const;
            void set(string_view key, string_view value);

            iterator begin();
            iterator end();
            const_iterator begin() const;
            const_iterator end() const;

        private:
            map<string, string> _headers;
    };
}