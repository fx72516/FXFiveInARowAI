#include "stdafx.h"
#include "fx_application.h"
#include "fx_chessboard.h"
#include "fx_fiar_player.h"
#include "fx_chessman.h"



FXApplication * FXApplication::m_instance = nullptr;

FXApplication::FXApplication()
	:m_chessboard(new FXChessboard(15, 15)), m_gameMode(FXGameMode(-1))
{
	m_fiarPlayers[0] = nullptr;
	m_fiarPlayers[1] = nullptr;

	SetGameMode(GM_PLAYER_VS_AI);

	m_fiarPlayers[0]->SetChessType(CHESS_WHITE);
}

FXApplication::~FXApplication()
{
}

FXApplication * FXApplication::Instance()
{
	if (!m_instance)
		m_instance = new FXApplication();
	return m_instance;
}

void FXApplication::Paint(const HDC & hdc, const RECT & rect)
{
	m_chessboard->Paint(hdc, rect);
}

void FXApplication::SetGameMode(FXGameMode gameMode)
{
	if (gameMode < GM_START || gameMode >= GM_END)
		return;

	m_gameMode = gameMode;

	UINT16 i = 0;
	while (i < gameMode)
	{
		if (!m_fiarPlayers[i])
			m_fiarPlayers[i] = new FXFIARPlayer();
		++i;
	}

	while (i < 2)
	{
		if (m_fiarPlayers[i])
		{
			delete m_fiarPlayers[i];
			m_fiarPlayers[i] = nullptr;
		}
		++i;
	}
}

bool FXApplication::MoveChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE * pType)
{
	if (!m_chessboard->MoveChessman(row, col))
		return false;

	bool isOver = false;
	if ((isOver = m_chessboard->CheckWinner(row, col)) && pType)
		m_chessboard->GetWinner(pType);
	return isOver;
}

void FXApplication::Restart()
{
	m_chessboard->Reset();

	if (m_gameMode == GM_PLAYER_VS_AI)
	{
		m_fiarPlayers[0]->SetChessType(!m_fiarPlayers[0]->ChessType());

		NextTurn();
	}
}

bool FXApplication::NextTurn()
{
	if (m_gameMode == GM_PLAYER_VS_AI)
	{
		if (m_fiarPlayers[0]->ChessType() == m_chessboard->nextType())
			return m_fiarPlayers[0]->Turn(m_chessboard);
	}

	return false;
}

bool FXApplication::Undo()
{
#ifdef TEST
	return m_chessboard->Undo();
#else
	return false;
#endif
}

LONG FXApplication::Random(const LONG & start, const LONG & end)
{
	static std::default_random_engine randEngine;
	std::uniform_int_distribution<> dist(start, end - 1);
	return dist(randEngine);
}

void FXApplication::OnMouseLeftButtonDown(const HWND & hWnd, const LONG & x, const LONG & y)
{
#ifndef TEST
	if (m_gameMode == GM_PLAYER_VS_AI && m_chessboard->nextType() == m_fiarPlayers[0]->ChessType())
		return;
#endif

	CHESS_POINT point = m_chessboard->GetChessPoint(x, y);

// 	if (m_chessboard->CheckPoint(point.row, point.col))
// 		m_chessboard->MoveChessman(point.row, point.col);

	if (MoveChessman(point.row, point.col) || NextTurn())
	{
		InvalidateRect(hWnd, NULL, false);

		CHESS_TYPE winner;
		m_chessboard->GetWinner(&winner);
		MessageBox(hWnd, winner ? _T("Black is winner!") : _T("White is winner!"), _T("win"), MB_OK);

		Restart();
	}

	InvalidateRect(hWnd, NULL, false);
}

void FXApplication::OnMouseRightButtonDown(const HWND & hWnd, const LONG & x, const LONG & y)
{
#ifdef TEST
	CHESS_POINT point = m_chessboard->GetChessPoint(x, y);

	if (m_chessboard->CheckPoint(point.row, point.col))
		m_fiarPlayers[0]->Test(m_chessboard, point.row, point.col, m_chessboard->nextType());
	else
		m_fiarPlayers[0]->Test(m_chessboard, m_chessboard->nextType());

	InvalidateRect(hWnd, NULL, false);
#endif
}
