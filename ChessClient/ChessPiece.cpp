#include "ChessPiece.h"
#include "ChessClient.h"
#include <qapplication.h>

ChessPiece::ChessPiece(ChessClient* Window, const EPieceType& InPawnType, const BoardVector2D& InitPosition, const ESide& InSide, bool bInIsOwn)
	: MainWindow(Window), PawnType(InPawnType), Side(InSide), bIsOwn(bInIsOwn)
{
	// Initialize label
	WidgetObject = new ClickableLabel(Window);
	WidgetObject->resize(59, 59);	

	QPixmap PawnImg(QString("assets/%1_%2.png").arg(PawnAssetData.at(PawnType)).arg(InSide == ESide::Light ? "w" : "b"));
	WidgetObject->setPixmap(PawnImg.scaled(WidgetObject->width(), WidgetObject->height(), Qt::KeepAspectRatio));

	SetPosition(InitPosition.X, InitPosition.Y);
	WidgetObject->show();

	connect(WidgetObject, &ClickableLabel::clicked, this, &ChessPiece::OnSelected);
}

void ChessPiece::SetPosition(int X, int Y)
{
	if (!WidgetObject) {
		qDebug() << "Pawn " << this << " Widget Object Invalid";
		return;
	}

	Position.X = X;
	Position.Y = Y;
	WidgetObject->move(240 + (X * 60), -30 + (Y * 60));
}

void ChessPiece::SetPosition(const BoardVector2D& Pos)
{
	if (!WidgetObject) {
		qDebug() << "Pawn " << this << " Widget Object Invalid";
		return;
	}

	Position = Pos;
	WidgetObject->move(240 + (Pos.X * 60), -30 + (Pos.Y * 60));
}

void ChessPiece::MoveTo(int X, int Y)
{
	for (int i = 0; i < MainWindow->VisibleAvailableSlots.size(); i++) {
		delete MainWindow->VisibleAvailableSlots[i];
	}

	MainWindow->VisibleAvailableSlots.clear();

	//SetPosition(X, Y);

	int PieceIdx = -1;
	for (int i = 0; i < MainWindow->ChessPieces.size(); i++) {
		if (MainWindow->ChessPieces[i] == this) {
			PieceIdx = i; 
			break;
		}
	}

	if (PieceIdx == -1) return;
	MainWindow->MovePiece(PieceIdx, X, Y);
}

void ChessPiece::Kill()
{
	if (WidgetObject) {
		delete WidgetObject;
	}

	delete this;
}

void ChessPiece::OnSelected()
{
	for (int i = 0; i < MainWindow->VisibleAvailableSlots.size(); i++) {
		delete MainWindow->VisibleAvailableSlots[i];
	}

	MainWindow->VisibleAvailableSlots.clear();

	if (!bIsOwn) return;

	std::vector<BoardVector2D> Slots;
	std::vector<ChessPiece*> Killable;

	GetAvailablePlaces(Slots, Killable);

	for (const BoardVector2D& Vec : Slots) {
		if (Vec.X > 8 || Vec.X < 1 || Vec.Y > 8 || Vec.Y < 0) continue;

		AvailableSlot* SlotLabel = new AvailableSlot(MainWindow);
		SlotLabel->RelatedPiece = this;
		SlotLabel->PosX = Vec.X;
		SlotLabel->PosY = Vec.Y;

		SlotLabel->resize(59, 59);

		QPixmap AImg("assets/available.png");
		SlotLabel->setPixmap(AImg.scaled(59, 59, Qt::KeepAspectRatio));

		SlotLabel->move(240 + Vec.X * 60, -30 + Vec.Y * 60);
		SlotLabel->show();

		MainWindow->VisibleAvailableSlots.push_back(SlotLabel);
	}

	for (ChessPiece* Piece : Killable) {
		if (Piece->GetSide() == Side) continue;

		AvailableSlot* SlotLabel = new AvailableSlot(MainWindow);
		SlotLabel->RelatedPiece = this;
		SlotLabel->PieceInSlot = Piece;
		SlotLabel->PosX = Piece->GetPosition().X;
		SlotLabel->PosY = Piece->GetPosition().Y;

		SlotLabel->resize(59, 59);

		QPixmap AImg("assets/killable.png");
		SlotLabel->setPixmap(AImg.scaled(59, 59, Qt::KeepAspectRatio));

		SlotLabel->move(240 + Piece->GetPosition().X * 60, -30 + Piece->GetPosition().Y * 60);
		SlotLabel->show();

		MainWindow->VisibleAvailableSlots.push_back(SlotLabel);
	}
}

void ChessPiece::GetAvailablePlaces(std::vector<BoardVector2D>& OutPlaces, std::vector<ChessPiece*>& OutKillable)
{
	switch (PawnType) {
	case EPieceType::King:
		// Iterate X and Y
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (x == 0 && y == 0) continue;

				ChessPiece* FirstHit = nullptr;
				if (MainWindow->IsAnyChessPieceInSlot(Position.X + x, Position.Y + y, FirstHit)) {
					OutKillable.push_back(FirstHit);
					continue;
				}

				OutPlaces.push_back(BoardVector2D(Position.X + x, Position.Y + y));
			}
		}
		break;

	case EPieceType::Queen:
		// X right
		AddIteratedSlots(1, 0, OutPlaces, OutKillable);

		// Y up
		AddIteratedSlots(0, -1, OutPlaces, OutKillable);

		// X left
		AddIteratedSlots(-1, 0, OutPlaces, OutKillable);

		// Y down
		AddIteratedSlots(0, 1, OutPlaces, OutKillable);

		// Diagonal right up
		AddIteratedSlots(1, -1, OutPlaces, OutKillable);

		// Diagonal left up
		AddIteratedSlots(-1, -1, OutPlaces, OutKillable);

		// Diagonal right down
		AddIteratedSlots(1, 1, OutPlaces, OutKillable);

		// Diagonal left down
		AddIteratedSlots(-1, 1, OutPlaces, OutKillable);
		break;

	case EPieceType::Rook:
		AddIteratedSlots(1, 0, OutPlaces, OutKillable);
		AddIteratedSlots(-1, 0, OutPlaces, OutKillable);
		AddIteratedSlots(0, 1, OutPlaces, OutKillable);
		AddIteratedSlots(0, -1, OutPlaces, OutKillable);
		break;
		
	case EPieceType::Bishop:
		AddIteratedSlots(1, -1, OutPlaces, OutKillable);
		AddIteratedSlots(1, 1, OutPlaces, OutKillable);
		AddIteratedSlots(-1, 1, OutPlaces, OutKillable);
		AddIteratedSlots(-1, -1, OutPlaces, OutKillable);
		break;

	case EPieceType::Knight:
		// Iterate X and Y axes
		for (int x = -2; x < 3; x++) {
			for (int y = -2; y < 3; y++) {
				if (abs(x) == 1 && abs(y) == 2 || abs(x) == 2 && abs(y) == 1) {
					ChessPiece* FirstHit = nullptr;
					if (MainWindow->IsAnyChessPieceInSlot(Position.X + x, Position.Y + y, FirstHit)) {
						OutKillable.push_back(FirstHit);
						continue;
					}

					OutPlaces.push_back(BoardVector2D(Position.X + x, Position.Y + y));
				}
			}
		}
		break;

	case EPieceType::Pawn:
		ChessPiece* FirstHit = nullptr;
		ChessPiece* SecondHit = nullptr;

		if (!MainWindow->IsAnyChessPieceInSlot(Position.X, Position.Y - 1, FirstHit)) {
			OutPlaces.push_back(BoardVector2D(Position.X, Position.Y - 1));
		}

		// If in starting position
		if (Position.Y == 7) {
			if (!MainWindow->IsAnyChessPieceInSlot(Position.X, Position.Y - 2, FirstHit)) {
				OutPlaces.push_back(BoardVector2D(Position.X, Position.Y - 2));
			}
		}
		
		// Check diagonal slots for enemy units
		if (MainWindow->IsAnyChessPieceInSlot(Position.X + 1, Position.Y - 1, FirstHit)) {
			OutKillable.push_back(FirstHit);
		}

		if (MainWindow->IsAnyChessPieceInSlot(Position.X - 1, Position.Y - 1, SecondHit)) {
			OutKillable.push_back(SecondHit);
		}
		break;
	}
}

void ChessPiece::AddIteratedSlots(int XDirection, int YDirection, std::vector<BoardVector2D>& OutPlaces, std::vector<ChessPiece*>& OutKillable)
{
	for (int i = 1; i < 9; i++) {
		ChessPiece* FirstHit = nullptr;
		if (MainWindow->IsAnyChessPieceInSlot(Position.X + XDirection * i, Position.Y + YDirection * i, FirstHit)) {
			OutKillable.push_back(FirstHit);
			break;
		}

		OutPlaces.push_back(BoardVector2D(Position.X + XDirection * i, Position.Y + YDirection * i));
	}
}
