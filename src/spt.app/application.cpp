module;

#include <wx/wx.h>

module spt.app:application;

import std;
import :window;
import :application;
import spt.infrastructure.net;
import spt.infrastructure.text;

using std::format;
using std::string;
using spt::application::Application;
using spt::application::ux::Window;
using spt::infrastructure::net::HttpClient;
using spt::infrastructure::net::HttpMethod;
using spt::infrastructure::net::HttpRequest;
using spt::infrastructure::net::HttpResponse;
using spt::infrastructure::text::JsonParser;
using spt::infrastructure::text::JsonValue;

bool Application::OnInit() {
    string apiKey { "2SACRS4UK8QY45Z9" };
    string company { "tencent" };
    string url { 
        format("https://www.alphavantage.co/query?function=SYMBOL_SEARCH&keywords={0}&apikey={1}", company, apiKey)
    };

    HttpRequest request { url, HttpMethod::GET };
    request.setHeader("Accept", "application/json");
    request.setHeader("User-Agent", "Blendwerk SPT/1.0");

    HttpClient client { };
    client.timeout(10L);
    
    HttpResponse response = { client.send(request) };
    if (!response.isSuccess()) {
        wxMessageBox(
            format("Failed to fetch data: HTTP {0}", response.status()),
            "Error",
            wxOK | wxICON_ERROR
        );
        return false;
    }

    JsonValue json { 
        JsonParser::parse(response.body()) 
    };

    Window* window = new Window { };
    window->Show(true);
    
    return true;
}
    
wxIMPLEMENT_APP(Application);