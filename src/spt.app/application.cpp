module;

#include <wx/wx.h>

export module spt.app:application;

import :window;
import spt.domain;
import spt.infrastructure;

namespace spt::application {
    using std::format;
    using std::string;
    using spt::application::ux::Window;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::text::JsonValue;

    export class Application : public wxApp {
        public:
            bool OnInit() override {
                Window* window = new Window { };
                window->Show(true);
                
                return true;
            }
    };

}

wxIMPLEMENT_APP(spt::application::Application);