module;

#include <wx/wx.h>

export module spt.app:application;

import :window;
import spt.domain;
import spt.infrastructure;

namespace spt::application {
    using std::string;
    using spt::application::ux::Window;

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