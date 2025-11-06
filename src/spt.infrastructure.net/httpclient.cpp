module;

#include <curl/curl.h>

module spt.infrastructure.net:httpclient;

import std;
import :httprequest;
import :httpresponse;
import :httpclient;

using std::format;
using std::getline;
using std::invalid_argument;
using std::istringstream;
using std::make_unique;
using std::move;
using std::runtime_error;
using std::string;
using std::string_view;
using std::unique_ptr;
using spt::infrastructure::net::HttpHeaders;
using spt::infrastructure::net::HttpClient;
using spt::infrastructure::net::HttpRequest;
using spt::infrastructure::net::HttpResponse;

HttpClient::HttpClient() 
    : _timeout { 30L }
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient() {
    curl_global_cleanup();
}

long HttpClient::timeout() const {
    return _timeout;
}

void HttpClient::timeout(long value) {
    if (value < 0L) {
        throw invalid_argument { 
            format("Timeout value cannot be negative: {0}", value) 
        };
    }
    _timeout = value;
}

HttpResponse HttpClient::send(const HttpRequest& request) const {
    curl_t curl { make_curl() };
    curl_list_t headerList { make_headers(request.headers()) };
    
    string responseBody { };
    string headerBuffer { };
    set_common_opts(curl, request.url(), _timeout, responseBody, headerBuffer, headerList);
    
    string body { request.body() };
    switch (request.method()) {
        case HttpMethod::POST:
            curl_easy_setopt(curl.get(), CURLOPT_POST, 1L);
            curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE, body.size());
            break;
        case HttpMethod::PUT:
            curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE, body.size());
            break;
        case HttpMethod::DEL:
            curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        case HttpMethod::GET:
        default:
            curl_easy_setopt(curl.get(), CURLOPT_HTTPGET, 1L);
            break;
    }

    CURLcode result { curl_easy_perform(curl.get()) };
    if (result != CURLE_OK) {
        throw runtime_error {
            curl_easy_strerror(result)
        };
    }

    long code { 0L };
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &code);

    HttpResponse response { 
        code, 
        move(responseBody), 
        parse_headers(headerBuffer)
    };
    
    return response;
}

HttpClient::curl_t HttpClient::make_curl() {
    CURL* raw = curl_easy_init();
    if (raw == nullptr) {
        throw runtime_error("Failed to initialize CURL");
    }

    return { raw, &curl_easy_cleanup };
}

HttpClient::curl_list_t HttpClient::make_headers(const HttpHeaders& headers) {
    struct curl_slist* list { nullptr };
    for (const auto& [k, v] : headers) {
        list = curl_slist_append(list, (k + ": " + v).c_str());
    }
    return { list, &curl_slist_free_all };
}

void HttpClient::set_common_opts(curl_t& curl, string_view url, long timeout, string& responseBody, string& headerBuffer, curl_list_t& headers)
{
    curl_easy_setopt(curl.get(), CURLOPT_URL, string { url }.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &headerBuffer);
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get());
    curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, "HttpClient/2.0");
    curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYHOST, 2L);
}

size_t HttpClient::write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

size_t HttpClient::header_callback(void* buffer, size_t size, size_t nitems, void* userp) {
    auto* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(buffer), size * nitems);
    return size * nitems;
}

HttpHeaders HttpClient::parse_headers(const string& raw) {
    HttpHeaders headers { };

    istringstream stream { raw };
    string line { };

    while (getline(stream, line)) {
        auto pos = line.find(':');
        if (pos != string::npos) {
            string key { line.substr(0, pos) };
            string value { line.substr(pos + 1) };
            key.erase(key.find_last_not_of(" \r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \r\n") + 1);
            headers.set(key, value);
        }
    }

    return headers;
}