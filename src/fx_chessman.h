#pragma once

class FXChessboard;

class FXChessman
{
	CHESS_TYPE		m_type;
	CHESS_POINT		m_position;
	bool			m_isLastChess;
	//FXChessboard *	m_chessboard;
public:
	FXChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE type, bool isLastChess = false);
	~FXChessman();

	CHESS_TYPE Type();

	void SetIsLastChess(bool isLastChess);

	void Paint(const HDC & hdc, const POINT & rect, const LONG & unit);
};

