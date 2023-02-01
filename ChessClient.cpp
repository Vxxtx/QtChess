#pragma once

#include "ChessClient.h"
#include "ClientConnection.h"
#include "ClickableLabel.h"

#include "qthread.h"
#include <QListWidgetItem>

ChessClient::ChessClient(QWidget *parent, const QString& Username)
    : QMainWindow(parent)
{
    LocalUsername = Username;

    ui_main.setupUi(this);
    
    connect(ui_main.send_btn, &QPushButton::pressed, this, &ChessClient::SendMsg);

    ConnectionThread = new QThread();
    Connection = new ClientConnection(LocalUsername, SendString);
    Connection->moveToThread(ConnectionThread);

    connect(ConnectionThread, &QThread::started, Connection, &ClientConnection::Init);
    connect(Connection, &ClientConnection::MessageReceived, this, &ChessClient::ConnectionMessageReceived);
    connect(this, &ChessClient::MsgSent, Connection, &ClientConnection::SendMsgFromClient);

    ConnectionThread->start();

    InitBoard();
}

ChessClient::~ChessClient()
{}

bool ChessClient::IsAnyChessPieceInSlot(int InX, int InY, ChessPiece*& FirstHit)
{
    bool bBlocking = false;

    for (ChessPiece* Piece : ChessPieces) {
    	if (Piece->GetPosition().X == InX && Piece->GetPosition().Y == InY) {
    		bBlocking = true;
            FirstHit = Piece;
    		break;
    	}
    }

    return bBlocking;
}

void ChessClient::InitBoard()
{
    // Fix board size
    QPixmap BoardImg("assets/board.png");
    ui_main.board->setPixmap(BoardImg.scaled(ui_main.board->width(), ui_main.board->height(), Qt::KeepAspectRatio));

    // Black side
    // Spawn special units
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPawn = new ChessPiece(this, SpawnData[i].PawnType, BoardVector2D(SpawnData[i].PosX, 1));
        NewPawn->SetSide(ESide::Black);
        ChessPieces.push_back(NewPawn);
    }

    // Spawn pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPawn = new ChessPiece(this, EPawnType::Pawn, BoardVector2D(i + 1, 2));
        NewPawn->SetSide(ESide::Black);
        ChessPieces.push_back(NewPawn);
    }

    // White side
    // Spawn special units
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPawn = new ChessPiece(this, SpawnData[i].PawnType, BoardVector2D(SpawnData[i].PosX, 8));
        NewPawn->SetSide(ESide::White);
        ChessPieces.push_back(NewPawn);
    }

    // Spawn pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPawn = new ChessPiece(this, EPawnType::Pawn, BoardVector2D(i + 1, 7));
        NewPawn->SetSide(ESide::White);
        ChessPieces.push_back(NewPawn);
    }
}

void ChessClient::SendMsg()
{
    QString Msg = ui_main.send_edit->toPlainText();
    SendString = Msg;
}

void ChessClient::ConnectionMessageReceived(const QString & Msg)
{
    qDebug() << "DEBUG: " << Msg << "\n";

    QListWidgetItem* NewItem = new QListWidgetItem();
    NewItem->setText(Msg);

    ui_main.chat_list->addItem(NewItem);
}
