export module spt.app:application;

import <wx/wx.h>;

namespace spt::application {
    export class Application : public wxApp {
        public:
            bool OnInit() override;
    };
}