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
    using std::string;
    using std::string_view;
    using std::vector;
    using std::optional;
    using std::format;
    using std::stod;
    using std::rand;
    using std::srand;
    using std::time;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::Company;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Money;
    using spt::domain::investments::Price;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::text::JsonValue;
    
    enum class MenuId {
        NewSession = wxID_HIGHEST + 1,
        Preferences
    };

    export class Window final : public wxFrame {
        public:
            Window()
                : wxFrame(nullptr, wxID_ANY, "Stock Portfolio Tracker"),
                  _portfolio(std::nullopt),
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
                
                Maximize();
            }

        private:
            void createMainPanel() {
                // Create main panel
                _mainPanel = new wxPanel(this, wxID_ANY);
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
                
                // Create splitter window
                _splitter = new wxSplitterWindow(_mainPanel, wxID_ANY);
                _splitter->SetMinimumPaneSize(200);
                
                // Left panel - Portfolio details
                _leftPanel = new wxPanel(_splitter, wxID_ANY);
                wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
                
                // Capital display
                _capitalText = new wxStaticText(_leftPanel, wxID_ANY, "Available Capital: $0.00");
                wxFont capitalFont = _capitalText->GetFont();
                capitalFont.SetPointSize(12);
                capitalFont.SetWeight(wxFONTWEIGHT_BOLD);
                _capitalText->SetFont(capitalFont);
                leftSizer->Add(_capitalText, 0, wxALL, 10);
                
                // Holdings grid
                _holdingsGrid = new wxGrid(_leftPanel, wxID_ANY);
                _holdingsGrid->CreateGrid(0, 5);
                _holdingsGrid->SetColLabelValue(0, "Symbol");
                _holdingsGrid->SetColLabelValue(1, "Shares");
                _holdingsGrid->SetColLabelValue(2, "Current Price");
                _holdingsGrid->SetColLabelValue(3, "Gain/Loss");
                _holdingsGrid->SetColLabelValue(4, "Total Value");
                _holdingsGrid->SetColSize(0, 100);
                _holdingsGrid->SetColSize(1, 80);
                _holdingsGrid->SetColSize(2, 120);
                _holdingsGrid->SetColSize(3, 120);
                _holdingsGrid->SetColSize(4, 120);
                _holdingsGrid->EnableEditing(false);
                _holdingsGrid->HideRowLabels();
                leftSizer->Add(_holdingsGrid, 1, wxEXPAND | wxALL, 10);
                
                _leftPanel->SetSizer(leftSizer);
                
                // Right panel - Price chart
                _rightPanel = new wxPanel(_splitter, wxID_ANY);
                wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
                
                wxStaticText* chartTitle = new wxStaticText(_rightPanel, wxID_ANY, "Price History");
                wxFont titleFont = chartTitle->GetFont();
                titleFont.SetPointSize(12);
                titleFont.SetWeight(wxFONTWEIGHT_BOLD);
                chartTitle->SetFont(titleFont);
                rightSizer->Add(chartTitle, 0, wxALL, 10);
                
                // Chart area
                _chartPanel = new wxPanel(_rightPanel, wxID_ANY);
                _chartPanel->SetBackgroundColour(*wxWHITE);
                _chartPanel->Bind(wxEVT_PAINT, &Window::onPaintChart, this);
                rightSizer->Add(_chartPanel, 1, wxEXPAND | wxALL, 10);
                
                _rightPanel->SetSizer(rightSizer);
                
                // Split the window
                _splitter->SplitVertically(_leftPanel, _rightPanel);
                _splitter->SetSashPosition(600);
                
                mainSizer->Add(_splitter, 1, wxEXPAND);
                _mainPanel->SetSizer(mainSizer);
                
                // Initially hide the panels until a portfolio is created
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
                PortfolioDialog dialog(this);
                
                if (dialog.ShowModal() == wxID_OK) {
                    auto opt = dialog.getPortfolio();
                    if (!opt.has_value()) {
                        wxMessageBox(
                            "No portfolio was created.",
                            "Error",
                            wxOK | wxICON_ERROR,
                            this
                        );
                        return;
                    }
                    
                    _portfolio = std::move(opt.value());
                    
                    // Show splitter and update layout
                    _splitter->Show();
                    _mainPanel->Layout();
                    _mainPanel->Refresh();
                    SendSizeEvent();
                    
                    // Update display with portfolio data
                    updatePortfolioDisplay();
                    
                    // Fetch price data from API
                    fetchIntradayData();
                    
                    SetStatusText("Portfolio loaded successfully.");
                }
            }
            
            void updatePortfolioDisplay() {
                if (!_portfolio.has_value()) return;
                
                // Update capital display
                _capitalText->SetLabel(wxString::Format("Available Capital: $%.2f", _portfolio->capital().value()));
                
                // Clear existing grid
                if (_holdingsGrid->GetNumberRows() > 0) {
                    _holdingsGrid->DeleteRows(0, _holdingsGrid->GetNumberRows());
                }
                
                // Populate holdings grid
                const auto& companies = _portfolio->companies();
                for (const auto& [ticker, company] : companies) {
                    _holdingsGrid->AppendRows(1);
                    int row = _holdingsGrid->GetNumberRows() - 1;
                    
                    _holdingsGrid->SetCellValue(row, 0, wxString(ticker.symbol()));
                    _holdingsGrid->SetCellValue(row, 1, wxString::Format("%d", company.shareCount()));
                    
                    // Get current price if available
                    if (!company.currentPrice().isZero()) {
                        _holdingsGrid->SetCellValue(row, 2, wxString::Format("$%.2f", company.currentPrice().value()));
                        // Calculate total value
                        double totalValue = company.currentPrice().value() * company.shareCount();
                        _holdingsGrid->SetCellValue(row, 4, wxString::Format("$%.2f", totalValue));
                    } else {
                        _holdingsGrid->SetCellValue(row, 2, "$0.00");
                        _holdingsGrid->SetCellValue(row, 4, "$0.00");
                    }
                    
                    _holdingsGrid->SetCellValue(row, 3, "0.00%");
                    
                    // Make all cells read-only
                    for (int col = 0; col < 5; col++) {
                        _holdingsGrid->SetReadOnly(row, col);
                    }
                }
                
                _holdingsGrid->AutoSize();
            }
            
            void fetchIntradayData() {
                if (!_portfolio.has_value()) return;
                
                const auto& companies = _portfolio->companies();
                
                if (companies.empty()) {
                    SetStatusText("No companies to fetch data for.");
                    return;
                }
                
                SetStatusText(wxString::Format("Fetching price data for %zu companies...", companies.size()));
                
                for (const auto& [ticker, company] : companies) {
                    fetchTickerData(ticker);
                }
                
                SetStatusText("Price data loaded.");
                updatePortfolioDisplay();
            }
            
            void fetchTickerData(const Ticker& ticker) {
                HttpClient client;
                string url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol="
                                + ticker.symbol()
                                + "&interval=5min&apikey=2SACRS4UK8QY45Z9";
                
                HttpRequest request { url, HttpMethod::GET };
                auto response = client.send(request);
                
                if (response.status() != 200) {
                    return;
                }
                
                // Parse JSON response
                JsonValue json = JsonParser::parse(response.body());
                
                if (!json.contains("Time Series (5min)")) {
                    return;
                }
                
                auto timeSeries = json["Time Series (5min)"].getObject();
                
                // Get the most recent price and update the portfolio
                for (const auto& [timestamp, data] : timeSeries) {
                    if (!data.contains("4. close")) continue;
                    
                    double closePrice = stod(data["4. close"].getString());
                    Price price { Money { closePrice } };
                    _portfolio->updatePrice(ticker, price);
                    
                    // Just use the most recent price
                    break;
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
            
            // Member variables
            wxPanel* _mainPanel;
            wxSplitterWindow* _splitter;
            wxPanel* _leftPanel;
            wxPanel* _rightPanel;
            wxPanel* _chartPanel;
            wxStaticText* _capitalText;
            wxGrid* _holdingsGrid;
            std::optional<Portfolio> _portfolio;
    };
}