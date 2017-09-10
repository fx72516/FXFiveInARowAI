#include "stdafx.h"
#include "fx_chessman.h"



FXChessman::FXChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE type, bool isLastChess)
	: m_position(row, col), m_type(type), m_isLastChess(isLastChess)
{
}

FXChessman::~FXChessman()
{
}

CHESS_TYPE FXChessman::Type()
{
	return m_type;
}

void FXChessman::SetIsLastChess(bool isLastChess)
{
	m_isLastChess = isLastChess;
}

void FXChessman::Paint(const HDC & dc, const POINT & pos, const LONG & unit)
{
	if (m_isLastChess)
	{
		LONG side = unit / 2 - 1;

		HPEN pen = CreatePen(PS_SOLID, 1, 0x7fff00);
		HGDIOBJ obj = SelectObject(dc, pen);
		Rectangle(dc, pos.x - side, pos.y - side, pos.x + side, pos.y + side);
		SelectObject(dc, obj);
	}

	HBRUSH brush = CreateSolidBrush(m_type ? 0 : 0xffffff);
	HBRUSH brs = (HBRUSH)SelectObject(dc, brush);
	LONG radio = unit / 2 - max((unit + 10) / 20, 1);
	Ellipse(dc, pos.x - radio, pos.y - radio, pos.x + radio, pos.y + radio);
	SelectObject(dc, brs);
}
