export module spt.app:application;

import <wx/wx.h>;

namespace spt {
    export class Application : public wxApp {
        public:
            bool OnInit() override;
    };
}