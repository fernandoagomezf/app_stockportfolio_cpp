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
        public:
            TickerSelectorDialog(wxWindow* parent)
                : wxDialog(parent, wxID_ANY, "Select Tickers", wxDefaultPosition, wxSize(700, 500))
            {
                initializeData();
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
            void initializeData() {
                _tickers = {
                    {"AAPL", "Apple Inc.", 175.43, false},
                    {"MSFT", "Microsoft Corporation", 378.91, false},
                    {"GOOGL", "Alphabet Inc.", 140.52, false},
                    {"AMZN", "Amazon.com Inc.", 145.33, false},
                    {"TSLA", "Tesla Inc.", 242.84, false},
                    {"META", "Meta Platforms Inc.", 338.14, false},
                    {"NVDA", "NVIDIA Corporation", 495.22, false},
                    {"BRK.B", "Berkshire Hathaway Inc.", 354.12, false},
                    {"JPM", "JPMorgan Chase & Co.", 152.73, false},
                    {"V", "Visa Inc.", 251.84, false}
                };
            }

            void createControls() {
                wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

                // Create grid
                _grid = new wxGrid(this, wxID_ANY);
                _grid->CreateGrid(_tickers.size(), 4);
                _grid->SetColLabelValue(0, "Select");
                _grid->SetColLabelValue(1, "Symbol");
                _grid->SetColLabelValue(2, "Name");
                _grid->SetColLabelValue(3, "Stock Price");

                // Set column properties
                _grid->SetColSize(0, 60);
                _grid->SetColSize(1, 100);
                _grid->SetColSize(2, 300);
                _grid->SetColSize(3, 120);

                // Populate grid
                for (size_t i = 0; i < _tickers.size(); ++i) {
                    _grid->SetCellRenderer(i, 0, new wxGridCellBoolRenderer());
                    _grid->SetCellEditor(i, 0, new wxGridCellBoolEditor());
                    _grid->SetCellValue(i, 0, "0");
                    
                    _grid->SetCellValue(i, 1, _tickers[i].symbol);
                    _grid->SetCellValue(i, 2, _tickers[i].name);
                    _grid->SetCellValue(i, 3, wxString::Format("$%.2f", _tickers[i].price));
                    
                    // Make non-checkbox cells read-only
                    _grid->SetReadOnly(i, 1);
                    _grid->SetReadOnly(i, 2);
                    _grid->SetReadOnly(i, 3);

                    // Right-align price column
                    _grid->SetCellAlignment(i, 3, wxALIGN_RIGHT, wxALIGN_CENTRE);
                }

                _grid->SetSelectionMode(wxGrid::wxGridSelectRows);
                _grid->EnableDragRowSize(false);
                _grid->DisableDragColSize();

                mainSizer->Add(_grid, 1, wxEXPAND | wxALL, 10);

                // Buttons
                wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
                
                wxButton* okButton = new wxButton(this, wxID_OK, "OK");
                wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

                buttonSizer->Add(okButton, 0, wxALL, 5);
                buttonSizer->Add(cancelButton, 0, wxALL, 5);

                mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);

                SetSizer(mainSizer);

                // Bind events
                okButton->Bind(wxEVT_BUTTON, &TickerSelectorDialog::onOk, this);
                cancelButton->Bind(wxEVT_BUTTON, &TickerSelectorDialog::onCancel, this);
            }

            void onOk(wxCommandEvent& event) {
                // Update ticker selection from grid
                for (size_t i = 0; i < _tickers.size(); ++i) {
                    _tickers[i].selected = (_grid->GetCellValue(i, 0) == "1");
                }

                // Check if at least one ticker is selected
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

            vector<TickerData> _tickers;
            wxGrid* _grid;
    };
}
