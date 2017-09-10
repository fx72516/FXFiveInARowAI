#pragma once

class FXChessboard;
class FXFIARPlayer;

enum FXGameMode
{
	GM_START			= 0,
	GM_PLAYER_VS_PLAYER	= GM_START,
	GM_PLAYER_VS_AI		= 1,
	GM_AI_VS_AI			= 2,
	GM_END
};

class FXApplication
{
	static FXApplication * m_instance;

	FXChessboard * m_chessboard;

	FXGameMode m_gameMode;
	FXFIARPlayer * m_fiarPlayers[2];

	FXApplication();
	//FXApplication(const FXApplication&);
public:
	~FXApplication();

	static FXApplication * Instance();

	void Paint(const HDC & hdc, const RECT & rect);

	void SetGameMode(FXGameMode gameMode);
	bool MoveChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE * type = nullptr);
	void Restart();
	bool NextTurn();

	bool Undo();
	static LONG Random(const LONG & start, const LONG & end);

	void OnMouseLeftButtonDown(const HWND & hWnd, const LONG & x, const LONG & y);
	void OnMouseRightButtonDown(const HWND & hWnd, const LONG & x, const LONG & y);
};

