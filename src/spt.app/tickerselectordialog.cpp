module;

#include <wx/wx.h>
#include <wx/grid.h>

export module spt.app:tickerselectordialog;

import std;

namespace spt::application::ux {
    using std::string;
    using std::vector;

    struct TickerData {
        string symbol;
        string name;
        double price;
        bool selected;
    };

    export class TickerSelectorDialog final : public wxDialog {
        private:
            vector<TickerData> _allTickers; 
            vector<TickerData> _tickers;    
            wxGrid* _grid;
            wxTextCtrl* _searchBox;

        public:
            TickerSelectorDialog(wxWindow* parent)
                : wxDialog(parent, wxID_ANY, "Select Tickers", wxDefaultPosition, wxSize(620, 500))
            {
                createControls();
                Centre();
            }

            vector<string> getSelectedSymbols() const {
                vector<string> selected;
                for (const auto& ticker : _tickers) {
                    if (ticker.selected) {
                        selected.push_back(ticker.symbol);
                    }
                }
                return selected;
            }

        private:
            void createControls() {
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

                wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
                wxStaticText* searchLabel = new wxStaticText(this, wxID_ANY, "Company to track:");
                _searchBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
                wxButton* searchButton = new wxButton(this, wxID_ANY, "Search");
                
                searchSizer->Add(searchLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
                searchSizer->Add(_searchBox, 1, wxEXPAND | wxRIGHT, 5);
                searchSizer->Add(searchButton, 0, wxALIGN_CENTER_VERTICAL);                
                mainSizer->Add(searchSizer, 0, wxEXPAND | wxALL, 10);

                _grid = new wxGrid(this, wxID_ANY);
                _grid->CreateGrid(_tickers.size(), 4);
                _grid->SetColLabelValue(0, "Select");
                _grid->SetColLabelValue(1, "Symbol");
                _grid->SetColLabelValue(2, "Name");
                _grid->SetColLabelValue(3, "Stock Price");
                _grid->SetColSize(0, 60);
                _grid->SetColSize(1, 100);
                _grid->SetColSize(2, 300);
                _grid->SetColSize(3, 120);
                _grid->SetSelectionMode(wxGrid::wxGridSelectRows);
                _grid->EnableDragRowSize(false);
                _grid->DisableDragColSize();
                _grid->HideRowLabels();

                populateGrid();

                mainSizer->Add(_grid, 1, wxEXPAND | wxALL, 10);

                wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);                
                wxButton* okButton = new wxButton(this, wxID_OK, "OK");
                wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

                buttonSizer->Add(okButton, 0, wxALL, 5);
                buttonSizer->Add(cancelButton, 0, wxALL, 5);
                mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);

                SetSizer(mainSizer);

                searchButton->Bind(wxEVT_BUTTON, &TickerSelectorDialog::onSearch, this);
                okButton->Bind(wxEVT_BUTTON, &TickerSelectorDialog::onOk, this);
                cancelButton->Bind(wxEVT_BUTTON, &TickerSelectorDialog::onCancel, this);
            }

            void onSearch(wxCommandEvent& event) {
                wxString searchTerm = _searchBox->GetValue().Lower();
                
                if (searchTerm.IsEmpty()) {
                    _tickers = _allTickers;
                } else {
                    _tickers.clear();
                    for (const auto& ticker : _allTickers) {
                        wxString symbol = wxString(ticker.symbol).Lower();
                        wxString name = wxString(ticker.name).Lower();
                        
                        if (symbol.Contains(searchTerm) || name.Contains(searchTerm)) {
                            _tickers.push_back(ticker);
                        }
                    }
                }
                
                populateGrid();
            }

            void populateGrid() {
                if (_grid->GetNumberRows() > 0) {
                    _grid->DeleteRows(0, _grid->GetNumberRows());
                }
                
                _grid->AppendRows(_tickers.size());                
                for (size_t i = 0; i < _tickers.size(); ++i) {
                    _grid->SetCellRenderer(i, 0, new wxGridCellBoolRenderer());
                    _grid->SetCellEditor(i, 0, new wxGridCellBoolEditor());
                    _grid->SetCellValue(i, 0, "");                      
                    _grid->SetCellValue(i, 1, _tickers[i].symbol);
                    _grid->SetCellValue(i, 2, _tickers[i].name);
                    _grid->SetCellValue(i, 3, wxString::Format("$%.2f", _tickers[i].price));                    
                    _grid->SetReadOnly(i, 1);
                    _grid->SetReadOnly(i, 2);
                    _grid->SetReadOnly(i, 3);
                    _grid->SetCellAlignment(i, 3, wxALIGN_RIGHT, wxALIGN_CENTRE);
                }
            }

            void onOk(wxCommandEvent& event) {
                if (_grid->IsCellEditControlEnabled()) {
                    _grid->DisableCellEditControl();
                }
                
                for (size_t i = 0; i < _tickers.size(); ++i) {
                    wxString cellValue = _grid->GetCellValue(i, 0);
                    _tickers[i].selected = !cellValue.IsEmpty() && cellValue != "0";
                }

                bool hasSelection = false;
                for (const auto& ticker : _tickers) {
                    if (ticker.selected) {
                        hasSelection = true;
                        break;
                    }
                }

                if (!hasSelection) {
                    wxMessageBox(
                        "Please select at least one ticker.",
                        "No Selection",
                        wxOK | wxICON_WARNING,
                        this
                    );
                    return;
                }

                EndModal(wxID_OK);
            }

            void onCancel(wxCommandEvent& event) {
                EndModal(wxID_CANCEL);
            }
    };
}
