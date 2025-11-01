module;

#include <wx/wx.h>

module spt.app:application;

import :window;
import :application;

using spt::Window;
using spt::Application;

wxIMPLEMENT_APP(Application);

bool Application::OnInit() {
    Window* window = new Window { };
    window->Show(true);

    return true;
}