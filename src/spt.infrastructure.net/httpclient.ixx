export module spt.infrastructure.net:httpclient;

import <curl/curl.h>;

import std;
import :httprequest;
import :httpresponse;

namespace spt::infrastructure::net {
    using std::string;
    using std::string_view;
    using std::unique_ptr;
    using spt::infrastructure::net::HttpHeaders;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;

    export class HttpClient final {
        public:
            HttpClient();
            ~HttpClient();

            long timeout() const;
            void timeout(long value);

            HttpResponse send(const HttpRequest& request) const;

        private:
            long _timeout;

            using curl_t = unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
            using curl_list_t = unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;

            static curl_t make_curl();
            static curl_list_t make_headers(const HttpHeaders& headers);
            static void set_common_opts(curl_t& curl, string_view url, long timeout, string& responseBody, string& headerBuffer, curl_list_t& headers);

            static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);
            static size_t header_callback(void* buffer, size_t size, size_t nitems, void* userp);
            static HttpHeaders parse_headers(const string& raw);
    };
}