module;

#include <wx/wx.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/grid.h>

module spt.app:window;

import :window;

using spt::Window;

Window::Window()
    : wxFrame(nullptr, wxID_ANY, "Stock Portfolio Tracker")
{
    Maximize();
}