module;

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/artprov.h>

export module spt.app:window;

import std;
import :tickerselectordialog;

namespace spt::application::ux {
    using std::string;
    using std::vector;
    
    enum class MenuId {
        NewSession = wxID_HIGHEST + 1,
        Preferences
    };

    export class Window final : public wxFrame {
        public:
            Window()
                : wxFrame(nullptr, wxID_ANY, "Stock Portfolio Tracker")
            {
                createMenuBar();
                createToolBar();
                createStatusBar();
                
                Maximize();
            }

        private:
            void createMenuBar() {
                wxMenuBar* menuBar = new wxMenuBar();

                wxMenu* fileMenu = new wxMenu();
                fileMenu->Append(static_cast<int>(MenuId::NewSession), "&New Session\tCtrl-N", "Create a new trading session");
                fileMenu->AppendSeparator();
                fileMenu->Append(wxID_EXIT, "&Exit\tAlt-F4", "Exit the application");
                menuBar->Append(fileMenu, "&File");

                wxMenu* editMenu = new wxMenu();
                editMenu->Append(static_cast<int>(MenuId::Preferences), "&Preferences\tCtrl-P", "Open preferences");
                menuBar->Append(editMenu, "&Edit");

                wxMenu* helpMenu = new wxMenu();
                helpMenu->Append(wxID_ABOUT, "&About\tF1", "About this application");
                menuBar->Append(helpMenu, "&Help");

                SetMenuBar(menuBar);

                Bind(wxEVT_MENU, &Window::onNewSession, this, static_cast<int>(MenuId::NewSession));
                Bind(wxEVT_MENU, &Window::onExit, this, wxID_EXIT);
                Bind(wxEVT_MENU, &Window::onPreferences, this, static_cast<int>(MenuId::Preferences));
                Bind(wxEVT_MENU, &Window::onAbout, this, wxID_ABOUT);
            }

            void createToolBar() {
                wxToolBar* toolBar = CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT);
                
                toolBar->AddTool(
                    static_cast<int>(MenuId::NewSession),
                    "New Session",
                    wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR),
                    "Create a new trading session"
                );
                
                toolBar->AddTool(
                    static_cast<int>(MenuId::Preferences),
                    "Preferences",
                    wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR),
                    "Open preferences"
                );

                toolBar->Realize();

                Bind(wxEVT_TOOL, &Window::onNewSession, this, static_cast<int>(MenuId::NewSession));
                Bind(wxEVT_TOOL, &Window::onPreferences, this, static_cast<int>(MenuId::Preferences));
            }

            void createStatusBar() {
                CreateStatusBar(1);
                SetStatusText("Ready");
            }

            void onExit(wxCommandEvent& event) {
                Close(true);
            }

            void onNewSession(wxCommandEvent& event) {
                TickerSelectorDialog dialog(this);
                
                if (dialog.ShowModal() == wxID_OK) {
                    vector<string> selectedSymbols = dialog.getSelectedSymbols();
                    
                    // Create comma-separated string
                    string symbolsStr;
                    for (size_t i = 0; i < selectedSymbols.size(); ++i) {
                        symbolsStr += selectedSymbols[i];
                        if (i < selectedSymbols.size() - 1) {
                            symbolsStr += ", ";
                        }
                    }
                    
                    wxMessageBox(
                        "Selected symbols: " + symbolsStr,
                        "New Session Created",
                        wxOK | wxICON_INFORMATION,
                        this
                    );
                    
                    SetStatusText("Session created with " + std::to_string(selectedSymbols.size()) + " ticker(s)");
                }
            }

            void onPreferences(wxCommandEvent& event) {
                wxMessageBox(
                    "Preferences dialog not yet implemented.",
                    "Preferences",
                    wxOK | wxICON_INFORMATION,
                    this
                );
            }

            void onAbout(wxCommandEvent& event) {
                wxMessageBox(
                    "Stock Portfolio Tracker\nVersion 1.0\n\nA modern C++ application for tracking stock portfolios.",
                    "About Stock Portfolio Tracker",
                    wxOK | wxICON_INFORMATION,
                    this
                );
            }
    };
}