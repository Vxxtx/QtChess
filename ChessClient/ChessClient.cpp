#pragma once

#include "ChessClient.h"
#include "ClientConnection.h"
#include "ClickableLabel.h"

#include "qthread.h"
#include <QListWidgetItem>
#include "ConnectionStatics.h"

ChessClient::ChessClient(QWidget *parent, const QString& Username)
    : QMainWindow(parent)
{
    LocalUsername = Username;

    ui_main.setupUi(this);
    
    // Fix board size
    QPixmap BoardImg("assets/board.png");
    ui_main.board->setPixmap(BoardImg.scaled(ui_main.board->width(), ui_main.board->height(), Qt::KeepAspectRatio));

    connect(ui_main.send_btn, &QPushButton::pressed, this, &ChessClient::SendMsg);

    ConnectionThread = new QThread();
    Connection = new ClientConnection(LocalUsername, SendString);
    Connection->moveToThread(ConnectionThread);

    connect(ConnectionThread, &QThread::started, Connection, &ClientConnection::Init);
    connect(Connection, &ClientConnection::MessageReceived, this, &ChessClient::ConnectionMessageReceived);
    connect(Connection, &ClientConnection::OnPlayerIDGot, this, &ChessClient::InitBoard);
    connect(Connection, &ClientConnection::OnPiecePositionUpdate, this, &ChessClient::UpdatePiecePosition);
    

    ConnectionThread->start();
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

void ChessClient::MovePiece(int MovedPieceIdx, int X, int Y)
{
    SendString = ConnectionStatics::PrepareString("pm", QString("%1:%2:%3").arg(MovedPieceIdx).arg(X).arg(Y));
}

void ChessClient::InitBoard(int PlayerID)
{
    bool bIsDark = (PlayerID == 0 ? ESide::Light : ESide::Dark) == ESide::Dark;

    // Spawn chess pieces in the following way to ensure indices are same for all players
    
    // Dark side
    // Spawn special units
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPiece = new ChessPiece(this, SpawnData[i].PieceType, BoardVector2D(SpawnData[i].PosX, bIsDark ? 8 : 1), ESide::Dark, bIsDark);
        ChessPieces.push_back(NewPiece);
    }

    // Spawn pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPiece = new ChessPiece(this, EPieceType::Pawn, BoardVector2D(i + 1, bIsDark ? 7 : 2), ESide::Dark, bIsDark);
        ChessPieces.push_back(NewPiece);
    }
    
    // White side
    // Spawn special units
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPiece = new ChessPiece(this, SpawnData[i].PieceType, BoardVector2D(SpawnData[i].PosX, !bIsDark ? 8 : 1), ESide::Light, !bIsDark);
        ChessPieces.push_back(NewPiece);
    }

    // Spawn pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* NewPiece = new ChessPiece(this, EPieceType::Pawn, BoardVector2D(i + 1, !bIsDark ? 7 : 2), ESide::Light, !bIsDark);
        ChessPieces.push_back(NewPiece);
    }
}

void ChessClient::UpdatePiecePosition(int MovedPieceIdx, int X, int Y)
{
    ChessPiece* Piece = ChessPieces[MovedPieceIdx];
    ChessPiece* HitPiece = nullptr;

    // Flip Y if piece is not own
    Y = Piece->IsOwn() ? Y : 9 - Y;

    if (Piece) {
        if (IsAnyChessPieceInSlot(X, Y, HitPiece)) {
            HitPiece->Kill();
        }

        ChessPieces[MovedPieceIdx]->SetPosition(X, Y);
    }
}

void ChessClient::SendMsg()
{
    QString Msg = ui_main.send_edit->toPlainText();
    SendString = ConnectionStatics::PrepareString("msg", Msg);
}

void ChessClient::ConnectionMessageReceived(const QString& Msg)
{
    qDebug() << "DEBUG: " << Msg << "\n";

    QListWidgetItem* NewItem = new QListWidgetItem();
    NewItem->setText(Msg);

    ui_main.chat_list->addItem(NewItem);
}
