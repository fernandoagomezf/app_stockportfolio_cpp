export module spt.app:window;

import <wx/wx.h>;
import <wx/grid.h>;

namespace spt::application::ux {
    export class Window final : public wxFrame {
        public:
            Window();
    };
}