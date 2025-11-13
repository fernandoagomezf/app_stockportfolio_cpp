module;

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/artprov.h>
#include <wx/splitter.h>

export module spt.app:window;

import std;
import spt.domain;
import spt.infrastructure;
import :portfoliodialog;

namespace spt::application::ux {
    using std::format;
    using std::move;
    using std::nullopt;
    using std::optional;
    using std::rand;
    using std::srand;
    using std::string;
    using std::string_view;
    using std::time;
    using std::vector;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::Company;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Money;
    using spt::domain::investments::Price;
    using spt::infrastructure::services::YahooPriceFetcher;
    
    enum class MenuId {
        NewSession = wxID_HIGHEST + 1,
        Refresh,
        Preferences
    };

    export class Window final : public wxFrame {
        private:
            wxPanel* _mainPanel;
            wxSplitterWindow* _splitter;
            wxPanel* _leftPanel;
            wxPanel* _rightPanel;
            wxPanel* _chartPanel;
            wxStaticText* _capitalText;
            wxGrid* _holdingsGrid;
            optional<Portfolio> _portfolio;
            YahooPriceFetcher _priceFetcher;

        public:
            Window()
                : wxFrame(nullptr, wxID_ANY, "Stock Portfolio Tracker"),
                  _portfolio(nullopt),
                  _mainPanel(nullptr),
                  _splitter(nullptr),
                  _leftPanel(nullptr),
                  _rightPanel(nullptr),
                  _chartPanel(nullptr),
                  _capitalText(nullptr),
                  _holdingsGrid(nullptr)
            {
                srand(static_cast<unsigned int>(time(nullptr)));
                
                createMenuBar();
                createToolBar();
                createStatusBar();
                createMainPanel();
                
                // Bind resize event to adjust grid columns
                Bind(wxEVT_SIZE, &Window::onResize, this);
                
                Maximize();
            }

        private:
            void createMainPanel() {
                _mainPanel = new wxPanel(this, wxID_ANY);
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
                
                _splitter = new wxSplitterWindow(_mainPanel, wxID_ANY);
                _splitter->SetMinimumPaneSize(200);
                
                _leftPanel = new wxPanel(_splitter, wxID_ANY);
                wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
                
                _capitalText = new wxStaticText(_leftPanel, wxID_ANY, "Available Capital: $0.00");
                wxFont capitalFont = _capitalText->GetFont();
                capitalFont.SetPointSize(12);
                capitalFont.SetWeight(wxFONTWEIGHT_BOLD);
                _capitalText->SetFont(capitalFont);
                leftSizer->Add(_capitalText, 0, wxALL, 10);
                
                _holdingsGrid = new wxGrid(_leftPanel, wxID_ANY);
                _holdingsGrid->CreateGrid(0, 7);
                _holdingsGrid->SetColLabelValue(0, "Symbol");
                _holdingsGrid->SetColLabelValue(1, "Company Name");
                _holdingsGrid->SetColLabelValue(2, "Exchange");
                _holdingsGrid->SetColLabelValue(3, "Shares");
                _holdingsGrid->SetColLabelValue(4, "Current Price");
                _holdingsGrid->SetColLabelValue(5, "Gain/Loss");
                _holdingsGrid->SetColLabelValue(6, "Total Value");
                _holdingsGrid->EnableEditing(false);
                _holdingsGrid->HideRowLabels();
                _holdingsGrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
                leftSizer->Add(_holdingsGrid, 1, wxEXPAND | wxALL, 10);                
                _leftPanel->SetSizer(leftSizer);
                
                _rightPanel = new wxPanel(_splitter, wxID_ANY);
                wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
                
                wxStaticText* chartTitle = new wxStaticText(_rightPanel, wxID_ANY, "Price History");
                wxFont titleFont = chartTitle->GetFont();
                titleFont.SetPointSize(12);
                titleFont.SetWeight(wxFONTWEIGHT_BOLD);
                chartTitle->SetFont(titleFont);
                rightSizer->Add(chartTitle, 0, wxALL, 10);
                
                _chartPanel = new wxPanel(_rightPanel, wxID_ANY);
                _chartPanel->SetBackgroundColour(*wxWHITE);
                _chartPanel->Bind(wxEVT_PAINT, &Window::onPaintChart, this);
                rightSizer->Add(_chartPanel, 1, wxEXPAND | wxALL, 10);                
                _rightPanel->SetSizer(rightSizer);
                
                _splitter->SplitVertically(_leftPanel, _rightPanel);
                _splitter->SetSashPosition(600);
                
                mainSizer->Add(_splitter, 1, wxEXPAND);
                _mainPanel->SetSizer(mainSizer);
                
                _splitter->Hide();
            }

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
                
                wxMenu* viewMenu = new wxMenu();
                viewMenu->Append(static_cast<int>(MenuId::Refresh), "&Refresh Prices\tF5", "Refresh intraday price data");
                menuBar->Append(viewMenu, "&View");

                wxMenu* helpMenu = new wxMenu();
                helpMenu->Append(wxID_ABOUT, "&About\tF1", "About this application");
                menuBar->Append(helpMenu, "&Help");

                SetMenuBar(menuBar);

                Bind(wxEVT_MENU, &Window::onNewSession, this, static_cast<int>(MenuId::NewSession));
                Bind(wxEVT_MENU, &Window::onRefresh, this, static_cast<int>(MenuId::Refresh));
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
                    static_cast<int>(MenuId::Refresh),
                    "Refresh",
                    wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR),
                    "Refresh intraday price data"
                );
                
                toolBar->Realize();

                Bind(wxEVT_TOOL, &Window::onNewSession, this, static_cast<int>(MenuId::NewSession));
                Bind(wxEVT_TOOL, &Window::onRefresh, this, static_cast<int>(MenuId::Refresh));
            }

            void createStatusBar() {
                CreateStatusBar(1);
                SetStatusText("Ready");
            }

            void onExit(wxCommandEvent& event) {
                Close(true);
            }

            void onNewSession(wxCommandEvent& event) {
                PortfolioDialog dialog(this);
                
                if (dialog.ShowModal() == wxID_OK) {
                    _portfolio = move(dialog.getPortfolio());                      
                    for (auto& ticker : _portfolio->tickers()) {
                        _portfolio->buyShares(ticker, 1);
                    }
                    _splitter->Show();
                    _mainPanel->Layout();
                    _mainPanel->Refresh();
                    SendSizeEvent();
                    updatePortfolioDisplay();  
                    fetchIntradayData();
                    SetStatusText("Portfolio loaded successfully.");
                }
            }
            
            void updatePortfolioDisplay() {
                if (!_portfolio.has_value()) return;
                
                _capitalText->SetLabel(wxString::Format("Available Capital: $%.2f", _portfolio->capital().value()));                
                if (_holdingsGrid->GetNumberRows() > 0) {
                    _holdingsGrid->DeleteRows(0, _holdingsGrid->GetNumberRows());
                }
                
                for (const auto& company : _portfolio->companies()) {
                    _holdingsGrid->AppendRows(1);
                    int row = _holdingsGrid->GetNumberRows() - 1;
                    
                    _holdingsGrid->SetCellValue(row, 0, wxString(company.ticker().symbol()));
                    _holdingsGrid->SetCellValue(row, 1, wxString(company.getName()));
                    _holdingsGrid->SetCellValue(row, 2, wxString(company.getExchange()));
                    _holdingsGrid->SetCellValue(row, 3, wxString::Format("%d", company.shareCount()));
                    
                    if (!company.currentPrice().amount().isZero()) {
                        _holdingsGrid->SetCellValue(row, 4, wxString::Format("$%.2f", company.currentPrice().amount().value()));
                        double totalValue = company.currentPrice().amount().value() * company.shareCount();
                        _holdingsGrid->SetCellValue(row, 6, wxString::Format("$%.2f", totalValue));
                    } else {
                        _holdingsGrid->SetCellValue(row, 4, "$0.00");
                        _holdingsGrid->SetCellValue(row, 6, "$0.00");
                    }                    
                    _holdingsGrid->SetCellValue(row, 5, "0.00%");
                    
                    for (int col = 0; col < 7; col++) {
                        _holdingsGrid->SetReadOnly(row, col);
                    }
                }
                
                // Size columns proportionally to fill available width
                resizeGridColumns();
            }
            
            void fetchIntradayData() {
                if (!_portfolio.has_value()) return;
                
                SetStatusText("Fetching price data...");
                
                try {
                    _priceFetcher.fetch(_portfolio.value());
                    SetStatusText("Price data loaded.");
                } catch (const std::exception& ex) {
                    SetStatusText(wxString::Format("Error fetching prices: %s", ex.what()));
                }
                
                updatePortfolioDisplay();
            }
            
            void resizeGridColumns() {
                if (!_holdingsGrid) return;
                
                int gridWidth = _holdingsGrid->GetClientSize().GetWidth();
                if (gridWidth > 100) {
                    _holdingsGrid->SetColSize(0, static_cast<int>(gridWidth * 0.11)); // Symbol: 10%
                    _holdingsGrid->SetColSize(1, static_cast<int>(gridWidth * 0.25)); // Company Name: 25%
                    _holdingsGrid->SetColSize(2, static_cast<int>(gridWidth * 0.12)); // Exchange: 12%
                    _holdingsGrid->SetColSize(3, static_cast<int>(gridWidth * 0.08)); // Shares: 8%
                    _holdingsGrid->SetColSize(4, static_cast<int>(gridWidth * 0.15)); // Current Price: 15%
                    _holdingsGrid->SetColSize(5, static_cast<int>(gridWidth * 0.12)); // Gain/Loss: 12%
                    _holdingsGrid->SetColSize(6, static_cast<int>(gridWidth * 0.15)); // Total Value: 15%
                }
            }
            
            void onResize(wxSizeEvent& event) {
                resizeGridColumns();
                event.Skip(); // Important: allow default processing
            }

            void onRefresh(wxCommandEvent& event) {
                if (!_portfolio.has_value()) {
                    wxMessageBox(
                        "No portfolio loaded. Please create a new session first.",
                        "No Portfolio",
                        wxOK | wxICON_INFORMATION,
                        this
                    );
                    return;
                }
                
                fetchIntradayData();
            }

            void onPreferences(wxCommandEvent& event) {
                wxMessageBox(
                    "Preferences dialog not yet implemented.",
                    "Preferences",
                    wxOK | wxICON_INFORMATION,
                    this
                );
            }
            
            void onPaintChart(wxPaintEvent& event) {
                wxPaintDC dc(_chartPanel);
                
                wxSize size = _chartPanel->GetSize();
                int width = size.GetWidth();
                int height = size.GetHeight();
                
                // Define margins
                int marginLeft = 60;
                int marginRight = 20;
                int marginTop = 20;
                int marginBottom = 40;
                
                int chartWidth = width - marginLeft - marginRight;
                int chartHeight = height - marginTop - marginBottom;
                
                if (chartWidth <= 0 || chartHeight <= 0) return;
                
                // Draw axes
                dc.SetPen(wxPen(*wxBLACK, 2));
                dc.DrawLine(marginLeft, marginTop, marginLeft, marginTop + chartHeight); // Y-axis
                dc.DrawLine(marginLeft, marginTop + chartHeight, marginLeft + chartWidth, marginTop + chartHeight); // X-axis
                
                // Draw grid lines
                dc.SetPen(wxPen(wxColour(220, 220, 220), 1));
                for (int i = 1; i < 5; i++) {
                    int y = marginTop + (chartHeight * i) / 5;
                    dc.DrawLine(marginLeft, y, marginLeft + chartWidth, y);
                }
                
                // Generate dummy data for 3 companies
                vector<wxColour> colors = {
                    wxColour(255, 0, 0),    // Red
                    wxColour(0, 128, 255),  // Blue
                    wxColour(0, 200, 0)     // Green
                };
                
                vector<string> companyNames = { "Company A", "Company B", "Company C" };
                
                int numPoints = 20;
                
                for (size_t companyIdx = 0; companyIdx < 3; companyIdx++) {
                    dc.SetPen(wxPen(colors[companyIdx], 2));
                    
                    // Generate dummy price data
                    vector<double> prices;
                    double basePrice = 100.0 + (companyIdx * 20.0);
                    
                    for (int i = 0; i < numPoints; i++) {
                        // Create some variation
                        double variation = (rand() % 20 - 10) * 0.5;
                        prices.push_back(basePrice + variation + (i * 0.5));
                    }
                    
                    // Find min/max for scaling
                    double minPrice = 80.0;
                    double maxPrice = 160.0;
                    
                    // Draw line chart
                    for (int i = 0; i < numPoints - 1; i++) {
                        int x1 = marginLeft + (chartWidth * i) / (numPoints - 1);
                        int x2 = marginLeft + (chartWidth * (i + 1)) / (numPoints - 1);
                        
                        // Invert Y because screen coordinates are top-down
                        int y1 = marginTop + chartHeight - (int)((prices[i] - minPrice) / (maxPrice - minPrice) * chartHeight);
                        int y2 = marginTop + chartHeight - (int)((prices[i + 1] - minPrice) / (maxPrice - minPrice) * chartHeight);
                        
                        dc.DrawLine(x1, y1, x2, y2);
                    }
                }
                
                // Draw legend
                dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                int legendY = marginTop + 10;
                for (size_t i = 0; i < 3; i++) {
                    dc.SetPen(wxPen(colors[i], 2));
                    dc.SetBrush(wxBrush(colors[i]));
                    dc.DrawLine(marginLeft + 10, legendY, marginLeft + 30, legendY);
                    dc.SetTextForeground(*wxBLACK);
                    dc.DrawText(companyNames[i], marginLeft + 35, legendY - 5);
                    legendY += 20;
                }
                
                // Draw axis labels
                dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                dc.SetTextForeground(*wxBLACK);
                
                // Y-axis labels (prices)
                for (int i = 0; i <= 5; i++) {
                    int y = marginTop + (chartHeight * i) / 5;
                    double price = 160.0 - (80.0 * i) / 5;
                    dc.DrawText(wxString::Format("$%.0f", price), 10, y - 8);
                }
                
                // X-axis label
                dc.DrawText("Time", marginLeft + chartWidth / 2 - 15, marginTop + chartHeight + 20);
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