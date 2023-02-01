#include "ChessEntry.h"
#include "ChessClient.h"

ChessEntry::ChessEntry(QWidget* parent)
    : QMainWindow(parent)
{
    ui_entry.setupUi(this);

    connect(ui_entry.connect_btn, &QPushButton::pressed, this, &ChessEntry::ConnectPressed);
}

ChessEntry::~ChessEntry()
{}

void ChessEntry::ConnectPressed()
{
    MW = new ChessClient(nullptr, ui_entry.usr_edit->text());
    MW->show();
    this->hide();
}
