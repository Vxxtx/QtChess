#pragma once

#include "ClickableLabel.h"

class ChessClient;
class ChessPiece;

class AvailableSlot : public ClickableLabel
{
public:
    explicit AvailableSlot(ChessClient* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~AvailableSlot();

protected:
    void mousePressEvent(QMouseEvent* event);

public:
    ChessPiece* RelatedPiece {nullptr};
    ChessPiece* PieceInSlot {nullptr};

    int PosX {1};
    int PosY {1};
};

