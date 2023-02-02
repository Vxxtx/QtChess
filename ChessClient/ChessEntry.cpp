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
    MW->move(pos().x() + width() / 2 - MW->width() / 2, pos().y()  + height() / 2 - MW->height() / 2);
    MW->show();
    this->hide();
}
