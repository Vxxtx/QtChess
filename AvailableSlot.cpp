#include "AvailableSlot.h"
#include "ChessClient.h"

AvailableSlot::AvailableSlot(ChessClient* parent, Qt::WindowFlags f)
	: ClickableLabel(parent) 
{
}

AvailableSlot::~AvailableSlot()
{
}

void AvailableSlot::mousePressEvent(QMouseEvent* event)
{
	RelatedPiece->MoveTo(PosX, PosY);
}
