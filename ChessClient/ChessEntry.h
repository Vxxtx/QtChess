#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientEntry.h"

class ChessClient;

class ChessEntry : public QMainWindow
{
    Q_OBJECT

public:
    ChessEntry(QWidget* parent = nullptr);
    ~ChessEntry();

private:
    Ui::entry_main ui_entry;

    // UI interactions
    void ConnectPressed();

    ChessClient* MW {nullptr};
};
