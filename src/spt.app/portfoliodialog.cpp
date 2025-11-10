module;

#include <wx/wx.h>
#include <wx/grid.h>

export module spt.app:portfoliodialog;

import std;
import spt.domain;
import spt.infrastructure;

namespace spt::application::ux {
    using std::string;
    using std::vector;
    using std::format;
    using std::optional;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::Company;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Money;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonValue;
    using spt::infrastructure::text::JsonParser;

    struct CompanyData {
        string symbol;
        string name;
        string type;
        string region;
        string currency;
    };

    export class PortfolioDialog final : public wxDialog {
        public:
            PortfolioDialog(wxWindow* parent)
                : wxDialog(parent, wxID_ANY, "Portfolio Information", wxDefaultPosition, wxSize(750, 600)),
                  _portfolio(std::nullopt)
            {
                createControls();
                Centre();
            }

            optional<Portfolio> getPortfolio() const {
                return _portfolio;
            }

        private:
            void createControls() {
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

                // Initial capital section
                wxStaticBoxSizer* capitalBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Porfolio");
                wxStaticText* capitalLabel = new wxStaticText(this, wxID_ANY, "Initial Capital Amount:");
                _capitalTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
                capitalBox->Add(capitalLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
                capitalBox->Add(_capitalTextCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
                mainSizer->Add(capitalBox, 0, wxEXPAND | wxALL, 10);

                // Company search section
                wxStaticBoxSizer* searchBox = new wxStaticBoxSizer(wxVERTICAL, this, "Company Search");
                wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
                wxStaticText* searchLabel = new wxStaticText(this, wxID_ANY, "Company Ticker:");
                _companySearchBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
                wxButton* searchButton = new wxButton(this, wxID_ANY, "Search && Add");
                
                searchSizer->Add(searchLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
                searchSizer->Add(_companySearchBox, 1, wxEXPAND | wxRIGHT, 5);
                searchSizer->Add(searchButton, 0, wxALIGN_CENTER_VERTICAL);
                searchBox->Add(searchSizer, 0, wxEXPAND | wxALL, 5);
                mainSizer->Add(searchBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

                // Grid for companies
                wxStaticBoxSizer* gridBox = new wxStaticBoxSizer(wxVERTICAL, this, "Companies to Track");
                _grid = new wxGrid(this, wxID_ANY);
                _grid->CreateGrid(0, 5);
                _grid->SetColLabelValue(0, "Symbol");
                _grid->SetColLabelValue(1, "Name");
                _grid->SetColLabelValue(2, "Type");
                _grid->SetColLabelValue(3, "Region");
                _grid->SetColLabelValue(4, "Currency");
                _grid->SetColSize(0, 100);
                _grid->SetColSize(1, 250);
                _grid->SetColSize(2, 120);
                _grid->SetColSize(3, 100);
                _grid->SetColSize(4, 80);
                _grid->EnableEditing(false);
                _grid->SetSelectionMode(wxGrid::wxGridSelectRows);
                _grid->EnableDragRowSize(false);
                _grid->DisableDragColSize();
                _grid->HideRowLabels();
                
                gridBox->Add(_grid, 1, wxEXPAND | wxALL, 5);
                
                // Remove button
                wxButton* removeButton = new wxButton(this, wxID_ANY, "Remove Selected");
                gridBox->Add(removeButton, 0, wxALIGN_RIGHT | wxALL, 5);
                
                mainSizer->Add(gridBox, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

                // OK/Cancel buttons
                wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
                wxButton* okButton = new wxButton(this, wxID_OK, "OK");
                wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
                buttonSizer->Add(okButton, 0, wxALL, 5);
                buttonSizer->Add(cancelButton, 0, wxALL, 5);
                mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);

                SetSizer(mainSizer);

                // Bind events
                searchButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onSearch, this);
                removeButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onRemove, this);
                okButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onOk, this);
                cancelButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onCancel, this);
            }

            void onSearch(wxCommandEvent& event) {
                wxString companyName = _companySearchBox->GetValue();
                
                if (companyName.IsEmpty()) {
                    wxMessageBox(
                        "Please enter a company name.",
                        "Empty Search",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                // Show busy cursor
                wxBusyCursor wait;

                // Search for company
                string company { companyName.ToStdString() };
                string apiKey { "2SACRS4UK8QY45Z9" };
                string url { 
                    format("https://www.alphavantage.co/query?function=SYMBOL_SEARCH&keywords={}&apikey={}", 
                           company, apiKey)
                };

                try {
                    HttpRequest request { url, HttpMethod::GET };
                    request.setHeader("Accept", "application/json");
                    request.setHeader("User-Agent", "Blendwerk SPT/1.0");

                    HttpClient client { };
                    client.timeout(10L);

                    HttpResponse response = client.send(request);
                    
                    if (!response.isSuccess()) {
                        wxMessageBox(
                            format("Failed to fetch data: HTTP {}", response.status()),
                            "Error",
                            wxOK | wxICON_ERROR,
                            this
                        );
                        return;
                    }

                    // Parse JSON response
                    JsonValue json { JsonParser::parse(response.body()) };
                    
                    if (!json.isObject()) {
                        wxMessageBox(
                            "Invalid response from API.",
                            "Error",
                            wxOK | wxICON_ERROR,
                            this
                        );
                        return;
                    }

                    if (!json.contains("bestMatches") || !json["bestMatches"].isArray()) {
                        wxMessageBox(
                            "No results found for the specified company.",
                            "Company Not Found",
                            wxOK | wxICON_WARNING,
                            this
                        );
                        return;
                    }

                    const auto& matches = json["bestMatches"].getArray();
                    if (matches.empty()) {
                        wxMessageBox(
                            "No results found for the specified company.",
                            "Company Not Found",
                            wxOK | wxICON_WARNING,
                            this
                        );
                        return;
                    }

                    // Get the best match (first result)
                    const auto& bestMatch = matches[0];
                    if (!bestMatch.isObject()) {
                        wxMessageBox(
                            "Invalid data format received.",
                            "Error",
                            wxOK | wxICON_ERROR,
                            this
                        );
                        return;
                    }
                    
                    CompanyData companyData;
                    companyData.symbol = bestMatch.contains("1. symbol") && bestMatch["1. symbol"].isString() 
                        ? bestMatch["1. symbol"].getString() : "";
                    companyData.name = bestMatch.contains("2. name") && bestMatch["2. name"].isString() 
                        ? bestMatch["2. name"].getString() : "";
                    companyData.type = bestMatch.contains("3. type") && bestMatch["3. type"].isString() 
                        ? bestMatch["3. type"].getString() : "";
                    companyData.region = bestMatch.contains("4. region") && bestMatch["4. region"].isString() 
                        ? bestMatch["4. region"].getString() : "";
                    companyData.currency = bestMatch.contains("8. currency") && bestMatch["8. currency"].isString() 
                        ? bestMatch["8. currency"].getString() : "";

                    // Check if company already exists
                    for (const auto& existing : _companies) {
                        if (existing.symbol == companyData.symbol) {
                            wxMessageBox(
                                "This company is already in the list.",
                                "Duplicate Company",
                                wxOK | wxICON_WARNING,
                                this
                            );
                            return;
                        }
                    }

                    // Add to list and grid
                    _companies.push_back(companyData);
                    addCompanyToGrid(companyData);

                    // Clear search box
                    _companySearchBox->Clear();

                } catch (const std::exception& ex) {
                    wxMessageBox(
                        format("Error: {}", ex.what()),
                        "Error",
                        wxOK | wxICON_ERROR,
                        this
                    );
                }
            }

            void addCompanyToGrid(const CompanyData& company) {
                int row = _grid->GetNumberRows();
                _grid->AppendRows(1);
                _grid->SetCellValue(row, 0, company.symbol);
                _grid->SetCellValue(row, 1, company.name);
                _grid->SetCellValue(row, 2, company.type);
                _grid->SetCellValue(row, 3, company.region);
                _grid->SetCellValue(row, 4, company.currency);
                
                // Make all cells read-only
                for (int col = 0; col < 5; ++col) {
                    _grid->SetReadOnly(row, col);
                }
            }

            void onRemove(wxCommandEvent& event) {
                wxArrayInt selectedRows = _grid->GetSelectedRows();
                
                if (selectedRows.IsEmpty()) {
                    wxMessageBox(
                        "Please select a company to remove.",
                        "No Selection",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                // Remove from back to front to avoid index issues
                for (int i = selectedRows.GetCount() - 1; i >= 0; --i) {
                    int row = selectedRows[i];
                    _companies.erase(_companies.begin() + row);
                    _grid->DeleteRows(row, 1);
                }
            }

            void onOk(wxCommandEvent& event) {
                // Validate initial capital
                wxString capitalStr = _capitalTextCtrl->GetValue();
                if (capitalStr.IsEmpty()) {
                    wxMessageBox(
                        "Please enter the initial capital.",
                        "Missing Capital",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                double capital = 0.0;
                if (!capitalStr.ToDouble(&capital) || capital <= 0) {
                    wxMessageBox(
                        "Please enter a valid positive number for the initial capital.",
                        "Invalid Capital",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                // Validate that at least one company is added
                if (_companies.empty()) {
                    wxMessageBox(
                        "Please add at least one company to track.",
                        "No Companies",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                // Create Portfolio object
                try {
                    Portfolio portfolio;
                    
                    // Set initial capital
                    Money initialCapital { capital };
                    portfolio.capitalize(initialCapital);
                    
                    // Track companies in portfolio
                    for (const auto& companyData : _companies) {
                        Ticker ticker { companyData.symbol };
                        portfolio.track(ticker);
                    }

                    _portfolio = portfolio;
                    EndModal(wxID_OK);

                } catch (const std::exception& ex) {
                    wxMessageBox(
                        format("Error creating portfolio: {}", ex.what()),
                        "Error",
                        wxOK | wxICON_ERROR,
                        this
                    );
                }
            }

            void onCancel(wxCommandEvent& event) {
                EndModal(wxID_CANCEL);
            }

            vector<CompanyData> _companies;
            wxGrid* _grid;
            wxTextCtrl* _capitalTextCtrl;
            wxTextCtrl* _companySearchBox;
            optional<Portfolio> _portfolio;
    };
}
