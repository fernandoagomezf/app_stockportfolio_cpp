export module spt.infrastructure.net:httpclient;

import <curl/curl.h>;

import std;
import :httprequest;
import :httpresponse;

namespace spt::infrastructure::net {
    using std::size_t;
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

            long timeout() const;
            void timeout(long value);

            HttpResponse send(const HttpRequest& request) const;

        private:
            long _timeout;

            using curl_t = unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
            using curl_list_t = unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;

            static curl_t makeCurl();
            static curl_list_t makeHeaders(const HttpHeaders& headers);

            static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
            static size_t headerCallback(void* buffer, size_t size, size_t nitems, void* userp);
            static HttpHeaders parseHeaders(const string& raw);
    };
}