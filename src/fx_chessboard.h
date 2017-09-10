#pragma once

class FXChessman;
struct CHESS_POINT;

class FXChessboard
{
	UINT16 m_rowCount;
	UINT16 m_columnCount;

	FXChessman ***	m_chessmans;

	struct Manual
	{
		CHESS_TYPE	type;
		CHESS_POINT	position;

		Manual(const UINT16 & row, const UINT16 & col, CHESS_TYPE _type)
			:position(row, col), type(_type)
		{}
	};
	std::vector<Manual> m_chessManual;

	LONG	m_unit;
	POINT	m_topLeft;

public:
	FXChessboard(const UINT16 & rowCount, const UINT16 & columnCount);
	~FXChessboard();

	void Paint(const HDC & dc, const RECT & rect);
	POINT PaintBoard(const HDC & dc, const RECT & rect, LONG & unit);

	bool MoveChessman(const UINT16 & row, const UINT16 & col);
	bool MoveChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE type);

	CHESS_TYPE nextType() const;
	FXChessman * lastChessman() const;
	FXChessman * GetChessman(const CHESS_POINT & pos) const;
	FXChessman * GetChessman(const UINT16 & row, const UINT16 & col) const;
	FXChessman * operator[](const CHESS_POINT & pos) const;

	UINT16 rowCount() const;
	UINT16 colCount() const;

	bool CheckWinner(CHESS_TYPE * type = nullptr) const;
	bool CheckWinner(const UINT16 & row, const UINT16 & col) const;
	bool GetWinner(CHESS_TYPE * type) const;
	bool IsOver() const;
	bool RandomPoint(UINT16 & row, UINT16 & col) const;
	bool CheckPoint(const UINT16 & row, const UINT16 & col) const;
	bool IsEmptyAll(const std::vector<CHESS_POINT> * pointList = nullptr) const;

	bool Undo();
	CHESS_POINT GetChessPoint(const LONG & x, const LONG & y) const;
	void Reset();

private:
#ifdef _DEBUG
	LONG ** m_weights;
#endif
	void InitWeightArray();
	void UninitWeightArray();
public:
	void SetPointWeight(const UINT16 & row, const UINT16 & col, const LONG & weight);
	void ClearPointWeights();
	void PaintWeights(const HDC & dc, const POINT & topLeft, const LONG & unit);
};

