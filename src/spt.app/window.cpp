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
    using std::chrono::system_clock;
    using std::exception;
    using std::format;
    using std::max;
    using std::min;
    using std::move;
    using std::nullopt;
    using std::numeric_limits;
    using std::optional;
    using std::pair;
    using std::rand;
    using std::srand;
    using std::string;
    using std::string_view;
    using std::time;
    using std::tm;
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
                
                resizeGridColumns();
            }
            
            void fetchIntradayData() {
                if (!_portfolio.has_value()) return;
                
                SetStatusText("Fetching price data...");
                
                try {
                    _priceFetcher.fetch(_portfolio.value());
                    SetStatusText("Price data loaded.");
                } catch (const exception& ex) {
                    SetStatusText(wxString::Format("Error fetching prices: %s", ex.what()));
                }
                
                updatePortfolioDisplay();
                repaintChart();                
            }

            void repaintChart() {
                if (_chartPanel) {
                    _chartPanel->Refresh();
                    _chartPanel->Update();
                }
            }
            
            void resizeGridColumns() {
                if (!_holdingsGrid) return;
                
                int gridWidth = _holdingsGrid->GetClientSize().GetWidth();
                if (gridWidth > 100) {
                    _holdingsGrid->SetColSize(0, static_cast<int>(gridWidth * 0.11)); // Symbol: 11%
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
                event.Skip(); // allow default processing
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
                
                int marginLeft = 60;
                int marginRight = 20;
                int marginTop = 20;
                int marginBottom = 140; // Increased to make room for timestamps and legend
                
                int chartWidth = width - marginLeft - marginRight;
                int chartHeight = height - marginTop - marginBottom;
                
                if (chartWidth <= 0 || chartHeight <= 0) return;                
                if (!_portfolio.has_value()) {
                    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                    dc.SetTextForeground(wxColour(128, 128, 128));
                    dc.DrawText("No portfolio data to display", width / 2 - 100, height / 2);
                    return;
                }
                
                dc.SetPen(wxPen(*wxBLACK, 2));
                dc.DrawLine(marginLeft, marginTop, marginLeft, marginTop + chartHeight); // Y-axis
                dc.DrawLine(marginLeft, marginTop + chartHeight, marginLeft + chartWidth, marginTop + chartHeight); // X-axis
                
                dc.SetPen(wxPen(wxColour(220, 220, 220), 1));
                for (int i = 1; i < 5; i++) {
                    int y = marginTop + (chartHeight * i) / 5;
                    dc.DrawLine(marginLeft, y, marginLeft + chartWidth, y);
                }
                
                vector<wxColour> colors = {
                    wxColour(255, 0, 0),      // Red
                    wxColour(0, 128, 255),    // Blue
                    wxColour(0, 200, 0),      // Green
                    wxColour(255, 128, 0),    // Orange
                    wxColour(128, 0, 255),    // Purple
                    wxColour(0, 200, 200),    // Cyan
                    wxColour(255, 0, 128),    // Pink
                    wxColour(128, 128, 0),    // Olive
                    wxColour(0, 128, 128),    // Teal
                    wxColour(255, 192, 0),    // Gold
                    wxColour(192, 0, 192),    // Magenta
                    wxColour(64, 128, 255),   // Light Blue
                    wxColour(255, 64, 0),     // Red-Orange
                    wxColour(128, 255, 0),    // Lime
                    wxColour(0, 64, 128)      // Navy
                };
                
                vector<string> companyNames;
                vector<vector<double>> companiesPrices;
                vector<pair<double, double>> companiesMinMax; // store min/max for each company
                vector<system_clock::time_point> timestamps;
                int maxDataPoints = 0;
                
                size_t companyIdx = 0;
                for (const auto& company : _portfolio->companies()) {
                    if (companyIdx >= colors.size()) break; // Limit to available colors
                    
                    auto history = company.priceHistory();
                    if (history.empty()) continue; // Skip companies with no price data
                    
                    companyNames.push_back(company.getName());
                    
                    vector<double> prices;
                    double companyMinPrice = numeric_limits<double>::max();
                    double companyMaxPrice = numeric_limits<double>::lowest();
                    
                    for (const auto& point : history) {
                        double priceValue = point.price().amount().value();
                        prices.push_back(priceValue);
                        companyMinPrice = min(companyMinPrice, priceValue);
                        companyMaxPrice = max(companyMaxPrice, priceValue);
                        if (companyIdx == 0) {
                            timestamps.push_back(point.stamp());
                        }
                    }
                    
                    companiesPrices.push_back(prices);
                    companiesMinMax.push_back({companyMinPrice, companyMaxPrice});
                    maxDataPoints = max(maxDataPoints, static_cast<int>(prices.size()));
                    companyIdx++;
                }
                
                if (companiesPrices.empty() || maxDataPoints == 0) {
                    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                    dc.SetTextForeground(wxColour(128, 128, 128));
                    dc.DrawText("No price data available", width / 2 - 80, height / 2);
                    return;
                }
                
                for (size_t idx = 0; idx < companiesPrices.size(); idx++) {
                    dc.SetPen(wxPen(colors[idx], 2));
                    dc.SetBrush(wxBrush(colors[idx]));
                    
                    const auto& prices = companiesPrices[idx];
                    int numPoints = static_cast<int>(prices.size());
                    
                    double companyMin = companiesMinMax[idx].first;
                    double companyMax = companiesMinMax[idx].second;
                    
                    double priceRange = companyMax - companyMin;
                    if (priceRange < 0.01) priceRange = 1.0; // avoid division by zero
                    companyMin -= priceRange * 0.1;
                    companyMax += priceRange * 0.1;
                    
                    if (numPoints > 1 && maxDataPoints > 1) {
                        for (int i = 0; i < numPoints - 1; i++) {
                            int x1 = marginLeft + (chartWidth * i) / (maxDataPoints - 1);
                            int x2 = marginLeft + (chartWidth * (i + 1)) / (maxDataPoints - 1);
                            
                            int y1 = marginTop + chartHeight - (int)((prices[i] - companyMin) / (companyMax - companyMin) * chartHeight);
                            int y2 = marginTop + chartHeight - (int)((prices[i + 1] - companyMin) / (companyMax - companyMin) * chartHeight);
                            
                            dc.DrawLine(x1, y1, x2, y2);
                        }
                    } else if (numPoints == 1) {
                        int x = marginLeft + chartWidth / 2;
                        int y = marginTop + chartHeight - (int)((prices[0] - companyMin) / (companyMax - companyMin) * chartHeight);
                        dc.DrawCircle(x, y, 3);
                    }
                }
                
                // Draw legend with price ranges below the chart in 3 columns
                dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                int legendStartY = marginTop + chartHeight + 35;
                int columnWidth = chartWidth / 3;
                
                for (size_t i = 0; i < companiesPrices.size(); i++) {
                    int column = i / 5; // 5 items per column
                    int row = i % 5;
                    
                    if (column >= 3) break; // Max 3 columns (15 items)
                    
                    int legendX = marginLeft + (column * columnWidth);
                    int legendY = legendStartY + (row * 20);
                    
                    dc.SetPen(wxPen(colors[i], 2));
                    dc.SetBrush(wxBrush(colors[i]));
                    dc.DrawLine(legendX + 10, legendY, legendX + 30, legendY);
                    dc.SetTextForeground(*wxBLACK);
                    
                    double minPrice = companiesMinMax[i].first;
                    double maxPrice = companiesMinMax[i].second;
                    wxString label = wxString::Format("%s ($%.2f - $%.2f)", 
                        companyNames[i].c_str(), minPrice, maxPrice);
                    dc.DrawText(label, legendX + 35, legendY - 5);
                }
                
                dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                dc.SetTextForeground(*wxBLACK);
                
                dc.DrawText("Price (normalized)", 5, marginTop - 10);
                
                if (!timestamps.empty() && maxDataPoints > 0) {
                    auto formatTime = [](system_clock::time_point tp) -> wxString {
                        auto timeT = system_clock::to_time_t(tp);
                        tm tm;
                        localtime_s(&tm, &timeT);
                        return wxString::Format("%02d:%02d", tm.tm_hour, tm.tm_min);
                    };
                    
                    if (maxDataPoints == 1) {
                        wxString timeStr = formatTime(timestamps[0]);
                        int x = marginLeft + chartWidth / 2;
                        dc.DrawText(timeStr, x - 15, marginTop + chartHeight + 20);
                    } else {
                        wxString leftTime = formatTime(timestamps[0]);
                        dc.DrawText(leftTime, marginLeft - 15, marginTop + chartHeight + 20);
                        
                        int midIdx = timestamps.size() / 2;
                        wxString midTime = formatTime(timestamps[midIdx]);
                        int midX = marginLeft + chartWidth / 2;
                        dc.DrawText(midTime, midX - 15, marginTop + chartHeight + 20);
                        
                        wxString rightTime = formatTime(timestamps[timestamps.size() - 1]);
                        int rightX = marginLeft + chartWidth;
                        dc.DrawText(rightTime, rightX - 30, marginTop + chartHeight + 20);
                    }
                }
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