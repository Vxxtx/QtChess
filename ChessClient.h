#pragma once

#include <QtWidgets/QMainWindow>
#include <qvector2d.h>
#include "ui_ChessClient.h"
#include "ui_ClientEntry.h"
#include "ChessPiece.h"

class ClientConnection;

struct PawnSpawnData {
    PawnSpawnData(const EPawnType& InPawnType, int X)
        : PawnType(InPawnType), PosX(X) {}

    EPawnType PawnType;
    int PosX;
};

class ChessClient : public QMainWindow
{
    Q_OBJECT

public:
    ChessClient(QWidget *parent = nullptr, const QString& Username = "");
    ~ChessClient();

signals:
    void MsgSent(const QString& Msg);

public slots:
    void ConnectionMessageReceived(const QString& Msg);

public:
    std::vector<AvailableSlot*> VisibleAvailableSlots {};
    std::vector<ChessPiece*> ChessPieces;

    bool IsAnyChessPieceInSlot(int X, int Y, ChessPiece*& FirstHit);

private:
    Ui::chess_main ui_main;

    QString LocalUsername;

    QThread* ConnectionThread{ nullptr };
    ClientConnection* Connection{ nullptr };

    QString SendString;
    
    const PawnSpawnData SpawnData[8] {
        PawnSpawnData(EPawnType::Rook, 1),
        PawnSpawnData(EPawnType::Knight, 2),
        PawnSpawnData(EPawnType::Bishop, 3),
        PawnSpawnData(EPawnType::Queen, 4),
        PawnSpawnData(EPawnType::King, 5),
        PawnSpawnData(EPawnType::Bishop, 6),
        PawnSpawnData(EPawnType::Knight, 7),
        PawnSpawnData(EPawnType::Rook, 8)
    };

    void InitBoard();
    void SendMsg();
};
