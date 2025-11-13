module;

#include <wx/wx.h>
#include <wx/grid.h>

export module spt.app:portfoliodialog;

import std;
import spt.domain;
import spt.infrastructure;

namespace spt::application::ux {
    using std::exception;
    using std::format;
    using std::make_unique;
    using std::nullopt;
    using std::optional;
    using std::string;
    using std::unique_ptr;
    using std::vector;
    using spt::domain::investments::Portfolio;
    using spt::domain::investments::Company;
    using spt::domain::investments::CompanySearch;
    using spt::domain::investments::Ticker;
    using spt::domain::investments::Money;
    using spt::infrastructure::net::HttpRequest;
    using spt::infrastructure::net::HttpResponse;
    using spt::infrastructure::net::HttpClient;
    using spt::infrastructure::net::HttpMethod;
    using spt::infrastructure::text::JsonValue;
    using spt::infrastructure::text::JsonParser;
    using spt::infrastructure::services::YahooCompanySearch;

    export class PortfolioDialog final : public wxDialog {
        private:
            wxGrid* _grid;
            wxTextCtrl* _capitalTextCtrl;
            wxTextCtrl* _companySearchBox;
            unique_ptr<CompanySearch> _searchService;
            Portfolio _portfolio;

        public:
            PortfolioDialog(wxWindow* parent)
                : wxDialog(parent, wxID_ANY, "Portfolio Information", wxDefaultPosition, wxSize(750, 600)),
                  _searchService { make_unique<YahooCompanySearch>() }
            {
                createControls();
                Centre();
            }

            Portfolio getPortfolio() const {
                return _portfolio;
            }

        private:
            void createControls() {
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

                wxStaticBoxSizer* capitalBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Porfolio");
                wxStaticText* capitalLabel = new wxStaticText(this, wxID_ANY, "Initial Capital Amount:");
                _capitalTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
                capitalBox->Add(capitalLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
                capitalBox->Add(_capitalTextCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
                mainSizer->Add(capitalBox, 0, wxEXPAND | wxALL, 10);

                wxStaticBoxSizer* searchBox = new wxStaticBoxSizer(wxVERTICAL, this, "Companies");
                wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
                wxStaticText* searchLabel = new wxStaticText(this, wxID_ANY, "Ticker:");
                _companySearchBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
                wxButton* searchButton = new wxButton(this, wxID_ANY, "Search && Add");
                
                searchSizer->Add(searchLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
                searchSizer->Add(_companySearchBox, 1, wxEXPAND | wxRIGHT, 5);
                searchSizer->Add(searchButton, 0, wxALIGN_CENTER_VERTICAL);
                searchBox->Add(searchSizer, 0, wxEXPAND | wxALL, 5);
                mainSizer->Add(searchBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

                wxStaticBoxSizer* gridBox = new wxStaticBoxSizer(wxVERTICAL, this, "Companies to Track");
                _grid = new wxGrid(this, wxID_ANY);
                _grid->CreateGrid(0, 6);
                _grid->SetColLabelValue(0, "Symbol");
                _grid->SetColLabelValue(1, "Exchange");
                _grid->SetColLabelValue(2, "Name");
                _grid->SetColLabelValue(3, "Type");
                _grid->SetColLabelValue(4, "Sector");
                _grid->SetColLabelValue(5, "Industry");
                _grid->SetColSize(0, 90);
                _grid->SetColSize(1, 100);
                _grid->SetColSize(2, 200);
                _grid->SetColSize(3, 70);
                _grid->SetColSize(4, 120);
                _grid->SetColSize(5, 120);
                _grid->EnableEditing(false);
                _grid->SetSelectionMode(wxGrid::wxGridSelectRows);
                _grid->EnableDragRowSize(false);
                _grid->DisableDragColSize();
                _grid->HideRowLabels();
                
                gridBox->Add(_grid, 1, wxEXPAND | wxALL, 5);
                
                wxButton* removeButton = new wxButton(this, wxID_ANY, "Remove Selected");
                gridBox->Add(removeButton, 0, wxALIGN_RIGHT | wxALL, 5);
                
                mainSizer->Add(gridBox, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

                wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
                wxButton* okButton = new wxButton(this, wxID_OK, "OK");
                wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
                buttonSizer->Add(okButton, 0, wxALL, 5);
                buttonSizer->Add(cancelButton, 0, wxALL, 5);
                mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);
                SetSizer(mainSizer);

                searchButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onSearch, this);
                removeButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onRemove, this);
                okButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onOk, this);
                cancelButton->Bind(wxEVT_BUTTON, &PortfolioDialog::onCancel, this);
            }

            void onSearch(wxCommandEvent& event) {
                wxBusyCursor wait;
                
                wxString companyName { _companySearchBox->GetValue() };
                if (companyName.IsEmpty()) {
                    wxMessageBox(
                        "Please enter a company ticker or name.",
                        "Empty Search",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                optional<Company> company { nullopt };
                try {
                    string term { companyName.ToStdString() };
                    company = _searchService->search(term);
                } catch (const exception& ex) {
                    wxMessageBox(
                        format("Error while searching: {}", ex.what()),
                        "Error",
                        wxOK | wxICON_ERROR,
                        this
                    );
                    return;
                }

                if (!company.has_value()) {
                    wxMessageBox(
                        "No company found with the specified symbol or name.",
                        "Company Not Found",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                _portfolio.track(company.value());
                addCompanyToGrid(company.value());
                _companySearchBox->Clear();
            }

            void addCompanyToGrid(const Company& company) {
                int row = _grid->GetNumberRows();
                _grid->AppendRows(1);
                _grid->SetCellValue(row, 0, company.ticker().symbol());
                _grid->SetCellValue(row, 1, company.getExchange());
                _grid->SetCellValue(row, 2, company.getName());
                _grid->SetCellValue(row, 3, company.getType());
                _grid->SetCellValue(row, 4, company.getSector());
                _grid->SetCellValue(row, 5, company.getIndustry());
                
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

                for (int i = selectedRows.GetCount() - 1; i >= 0; --i) {
                    int row = selectedRows[i];
                    wxString symbol = _grid->GetCellValue(row, 0);
                    if (!symbol.IsEmpty()) {                        
                        Ticker ticker { symbol.ToStdString() };
                        _portfolio.untrack(ticker);
                    }
                    _grid->DeleteRows(row, 1);
                }
            }

            void onOk(wxCommandEvent& event) {                
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

                if (_grid->GetNumberRows() == 0) {
                    wxMessageBox(
                        "Please add at least one company to track.",
                        "No Companies",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                try {
                    Money initialCapital { capital };
                    _portfolio.capitalize(initialCapital);                    
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
    };
}
