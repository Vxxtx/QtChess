#pragma once
#include <qobject.h>
#include <qmainwindow.h>
#include <qvector2d.h>
#include <vector>
#include "ClickableLabel.h"
#include "AvailableSlot.h"

class ChessClient;

struct BoardVector2D {
public:
	BoardVector2D(int InX = 1, int InY = 1) 
		: X(InX), Y(InY) {}
	int X = 1;
	int Y = 1;

	bool operator==(const BoardVector2D& InBoardVec) {
		return (X == InBoardVec.X && Y == InBoardVec.Y);
	};
};

enum class EPieceType {
	King, 
	Queen, 
	Rook,
	Bishop,
	Knight,
	Pawn
};

enum class ESide {
	Light,
	Dark
};

class ChessPiece : public QObject
{
	Q_OBJECT

public:
	ChessPiece(ChessClient* Window, const EPieceType& InPawnType, const BoardVector2D& InitPosition, const ESide& InSide, bool bInIsOwn);

	void SetPosition(int X, int Y);
	void SetPosition(const BoardVector2D& Pos);
	void MoveTo(int X, int Y);

	inline const BoardVector2D GetPosition() { return Position;  };

	inline void SetSide(const ESide& InSide) { Side = InSide; };
	inline ESide GetSide() { return Side; };
	inline bool IsOwn() { return bIsOwn; };

	void Kill();

private:
	ClickableLabel* WidgetObject {nullptr};
	ChessClient* MainWindow {nullptr};

	const std::map<EPieceType, QString> PawnAssetData{
		{EPieceType::King, "k"},
		{EPieceType::Queen, "q"},
		{EPieceType::Rook, "r"},
		{EPieceType::Bishop, "b"},
		{EPieceType::Knight, "kn"},
		{EPieceType::Pawn, "p"},
	};

	EPieceType PawnType {EPieceType::Pawn};
	ESide Side {ESide::Light};
	BoardVector2D Position {1, 1};
	bool bIsOwn {false};

	void OnSelected();

	void GetAvailablePlaces(std::vector<BoardVector2D>& OutPlaces, std::vector<ChessPiece*>& OutKillable);
	void AddIteratedSlots(int XDirection, int YDirection, std::vector<BoardVector2D>& OutPlaces, std::vector<ChessPiece*>& OutKillable);
};