#pragma once

#include <QtWidgets/QMainWindow>
#include <qvector2d.h>
#include "ui_ChessClient.h"
#include "ui_ClientEntry.h"
#include "ChessPiece.h"

class ClientConnection;

struct PieceSpawnData {
    PieceSpawnData(const EPieceType& InPieceType, int X)
        : PieceType(InPieceType), PosX(X) {}

    EPieceType PieceType;
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
    void OnPieceMoved(int MovedPieceIdx, int X, int Y);

public slots:
    void ConnectionMessageReceived(const QString& Msg);
    void InitBoard(int PlayerID);
    void UpdatePiecePosition(int MovedPieceIdx, int X, int Y);

public:
    std::vector<AvailableSlot*> VisibleAvailableSlots {};
    std::vector<ChessPiece*> ChessPieces;

    bool IsAnyChessPieceInSlot(int X, int Y, ChessPiece*& FirstHit);
    void MovePiece(int MovedPieceIdx, int X, int Y);

private:
    Ui::chess_main ui_main;

    QString LocalUsername;

    QThread* ConnectionThread{ nullptr };
    ClientConnection* Connection{ nullptr };

    QString SendString;
    
    const PieceSpawnData SpawnData[8] {
        PieceSpawnData(EPieceType::Rook, 1),
        PieceSpawnData(EPieceType::Knight, 2),
        PieceSpawnData(EPieceType::Bishop, 3),
        PieceSpawnData(EPieceType::Queen, 4),
        PieceSpawnData(EPieceType::King, 5),
        PieceSpawnData(EPieceType::Bishop, 6),
        PieceSpawnData(EPieceType::Knight, 7),
        PieceSpawnData(EPieceType::Rook, 8)
    };

    void SendMsg();
};
