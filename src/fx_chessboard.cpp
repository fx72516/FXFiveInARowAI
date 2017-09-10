#include "stdafx.h"
#include "fx_chessboard.h"
#include "fx_chessman.h"
#include "fx_application.h"
#include <cassert>

#define BEGIN_FOREACH_ROW(i)	for (UINT16 (i) = 0; (i) < m_rowCount; ++(i)) {
#define BEGIN_FOREACH_COLUMN(i)	for (UINT16 (i) = 0; (i) < m_columnCount; ++(i)) {
#define END_FOREACH				}

FXChessboard::FXChessboard(const UINT16 & rowCount, const UINT16 & columnCount)
	: m_rowCount(rowCount), m_columnCount(columnCount)
{
	m_chessmans = new FXChessman ** [m_rowCount];
	BEGIN_FOREACH_ROW(i)
	m_chessmans[i] = new FXChessman * [m_columnCount];
	BEGIN_FOREACH_COLUMN(j)
	m_chessmans[i][j] = nullptr;
	END_FOREACH
	END_FOREACH

	InitWeightArray();

	m_topLeft = { -1, -1 };
}

FXChessboard::~FXChessboard()
{
	m_chessManual.clear();

	UninitWeightArray();

	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	if (m_chessmans[i][j])
	{
		delete m_chessmans[i][j];
		m_chessmans[i][j] = nullptr;
	}
	END_FOREACH
	delete[] m_chessmans[i];
	END_FOREACH
	delete[] m_chessmans;
}

void FXChessboard::Paint(const HDC & dc, const RECT & rect)
{
	LONG unit = 0;
	POINT topLeft = PaintBoard(dc, rect, unit);

	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	if (m_chessmans[i][j])
	{
		POINT pos = { topLeft.x + unit * i, topLeft.y + unit * j};
		m_chessmans[i][j]->Paint(dc, pos, unit);
	}
	END_FOREACH
	END_FOREACH

	PaintWeights(dc, topLeft, unit);
}

POINT FXChessboard::PaintBoard(const HDC & dc, const RECT & rect, LONG & unit)
{
	m_unit = unit = min((rect.bottom - rect.top) / (m_rowCount), (rect.right - rect.left) / (m_columnCount));

	LONG marginH = (rect.right - rect.left - unit * (m_columnCount - 1)) / 2;
	LONG marginV = (rect.bottom - rect.top - unit * (m_rowCount - 1)) / 2;

	// HPEN pen = CreatePen(PS_SOLID, 1, 0);
	// HPEN p = (HPEN)SelectObject(dc, pen);

	Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);

	for (UINT16 i = 0; i < m_rowCount; ++i)
	{
		POINT point;
		MoveToEx(dc, i * unit + marginH, rect.left + marginV, &point);
		LineTo(dc, i * unit + marginH, rect.left + marginV + unit * (m_columnCount - 1));
	}

	for (UINT16 i = 0; i < m_columnCount; ++i)
	{
		POINT point;
		MoveToEx(dc, rect.top + marginH, i * unit + marginV, &point);
		LineTo(dc, rect.top + marginH + unit * (m_rowCount - 1), i * unit + marginV);
	}

	// SelectObject(dc, p);

	m_topLeft = { rect.left + marginH, rect.top + marginV };
	return m_topLeft;
}

CHESS_TYPE FXChessboard::nextType() const
{
	return m_chessManual.size() > 0 ? !m_chessManual[m_chessManual.size() - 1].type : CHESS_BLACK;
}

FXChessman * FXChessboard::lastChessman() const
{
	if (m_chessManual.size() == 0)
		return nullptr;

	CHESS_POINT lastPoint = m_chessManual[m_chessManual.size() - 1].position;
	return m_chessmans[lastPoint.row][lastPoint.col];
}

FXChessman * FXChessboard::GetChessman(const CHESS_POINT & pos) const
{
	return GetChessman(pos.row, pos.col);
}

FXChessman * FXChessboard::GetChessman(const UINT16 & row, const UINT16 & col) const
{
	if (row >= m_rowCount || col >= m_columnCount)
		return nullptr;

	return m_chessmans[row][col];
}

FXChessman * FXChessboard::operator[](const CHESS_POINT & pos) const
{
	return GetChessman(pos);
}

UINT16 FXChessboard::rowCount() const
{
	return m_rowCount;
}

UINT16 FXChessboard::colCount() const
{
	return m_columnCount;
}

bool FXChessboard::MoveChessman(const UINT16 & row, const UINT16 & col)
{
	return MoveChessman(row, col, nextType());
}

bool FXChessboard::MoveChessman(const UINT16 & row, const UINT16 & col, CHESS_TYPE type)
{
	if (row >= m_rowCount || col >= m_columnCount || m_chessmans[row][col])
		return false;

	m_chessmans[row][col] = new FXChessman(row, col, type, true);

	if (m_chessManual.size() > 0)
	{
		const CHESS_POINT & lastPoint = m_chessManual[m_chessManual.size() - 1].position;
		m_chessmans[lastPoint.row][lastPoint.col]->SetIsLastChess(false);
	}

	m_chessManual.push_back(Manual(row, col, type));

	return true;
}

bool FXChessboard::CheckWinner(CHESS_TYPE * pType) const
{
	auto CheckFiveInARow = [&](const UINT16 & row, const UINT16 & col)
	{
		if (!m_chessmans[row][col])
			return false;

		UINT16 lineCount[] = { 1, 1, 1, 1 };
		CHESS_TYPE baseType = m_chessmans[row][col]->Type();
		for (INT16 i = 1; i < FIVE; ++i)
		{
			for (UINT16 j = 0; j < 4; ++j)
			{
				UINT16 r = row + i * (j != 0 ? j >> 1 : -1);
				UINT16 c = col + i * (j != 0 ? j & 1 : 1);
				if (r < m_rowCount && c < m_columnCount && lineCount[j] && m_chessmans[r][c] && m_chessmans[r][c]->Type() == baseType)
				{
					if (++lineCount[j] >= 5 && pType)
						return (*pType = baseType) || true;
				}
				else
				{
					lineCount[j] = 0;
					if (!(lineCount[0] || lineCount[1] || lineCount[2] || lineCount[3]))
						return false;
				}
			}
		}

		return lineCount[0] || lineCount[1] || lineCount[2] || lineCount[3];
	};

	for (UINT16 i = 0; i < m_rowCount; ++i)
	{
		for (UINT16 j  = 0; j < m_columnCount; ++j)
		{
			if (CheckFiveInARow(i, j))
				return true;
		}
	}

	return false;
}

bool FXChessboard::CheckWinner(const UINT16 & row, const UINT16 & col) const
{
	CHECK_ASSERT_RET_FALSE(CheckPoint(row, col))
	CHECK_ASSERT_RET_FALSE(m_chessmans[row][col]);

	// bool(*_CheckFiveInARow)(bool dirRow, bool dirCol) = [&row, &col](bool dirRow, bool dirCol){return false;};
	auto CheckFiveInARow = [&](INT16 dirRow, INT16 dirCol)
	{
		UINT16 lineCount = 1;
		CHESS_TYPE baseType = m_chessmans[row][col]->Type();
		INT16 dir[] = { 1, -1 };
		for (INT16 d = 0; d < 2; ++d)
		{
			for (INT16 i = 1; i < FIVE; ++i)
			{
				INT16 r = (INT16)row + i * dirRow * dir[d];
				INT16 c = (INT16)col + i * dirCol * dir[d];
				if (r < 0 || r >= m_rowCount || c < 0 || c >= m_columnCount)
					break;

				if (m_chessmans[r][c] && m_chessmans[r][c]->Type() == baseType)
					++lineCount;
				else
					break;
			}
		}

		return lineCount >= FIVE;
	};

	INT16 dir[][2] = { { 1, 0 }, {0, 1}, {1, 1}, {1, -1} };
	for (UINT16 i = 0; i < 4; ++i)
	{
		if (CheckFiveInARow(dir[i][0], dir[i][1]))
			return true;
	}

	return false;
}

bool FXChessboard::GetWinner(CHESS_TYPE * pType) const
{
	ASSERT(CheckWinner());
	ASSERT(m_chessManual.size() > 0);
	if (m_chessManual.size() <= 9 || !CheckWinner())
		return false;

	if (pType)
		*pType = m_chessManual[m_chessManual.size() - 1].type;
	return true;
}

bool FXChessboard::IsOver() const
{
	return m_chessManual.size() == m_rowCount * m_columnCount;
}

bool FXChessboard::RandomPoint(UINT16 & row, UINT16 & col) const
{
	if (IsOver())
		return false;

	int randIndex = FXApplication::Random(0, m_rowCount * m_columnCount - m_chessManual.size());

	int idx = 0;
	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	if (!m_chessmans[i][j])
	{
		if (idx == randIndex)
		{
			row = i;
			col = j;
			return true;
		}
		++idx;
	}
	END_FOREACH
	END_FOREACH

	ASSERT(false);
	return false;
}

bool FXChessboard::CheckPoint(const UINT16 & row, const UINT16 & col) const
{
	return row < m_rowCount && col < m_columnCount;
}

bool FXChessboard::IsEmptyAll(const std::vector<CHESS_POINT>* pointList) const
{
	if (pointList)
	{
		for (UINT16 i = 0; i < pointList->size(); ++i)
		{
			if (GetChessman(pointList->at(i)))
				return false;
		}
		return true;
	}
	return m_chessManual.size() == 0;
}

bool FXChessboard::Undo()
{
	if (m_chessManual.size() == 0)
		return false;

	CHESS_POINT lastPoint = m_chessManual[m_chessManual.size() - 1].position;

	delete m_chessmans[lastPoint.row][lastPoint.col];
	m_chessmans[lastPoint.row][lastPoint.col] = nullptr;

	m_chessManual.erase(m_chessManual.end() - 1);

	return true;
}

CHESS_POINT FXChessboard::GetChessPoint(const LONG & x, const LONG & y) const
{
	if (m_unit <= 0)
		return { m_rowCount, m_columnCount };

	UINT16 row = UINT16((x - m_topLeft.x + m_unit / 2) / m_unit);
	UINT16 col = UINT16((y - m_topLeft.y + m_unit / 2) / m_unit);

	return CHESS_POINT(row, col);
}

void FXChessboard::Reset()
{
	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	if (m_chessmans[i][j])
	{
		delete m_chessmans[i][j];
		m_chessmans[i][j] = nullptr;
	}
	END_FOREACH
	END_FOREACH

	ClearPointWeights();
	m_chessManual.clear();
}

void FXChessboard::InitWeightArray()
{
#ifdef _DEBUG
	m_weights = new LONG*[m_rowCount];
	BEGIN_FOREACH_ROW(i)
	m_weights[i] = new LONG[m_columnCount];
	BEGIN_FOREACH_COLUMN(j)
	m_weights[i][j] = 0;
	END_FOREACH
	END_FOREACH
#endif // _DEBUG
}

void FXChessboard::UninitWeightArray()
{
#ifdef _DEBUG
	BEGIN_FOREACH_ROW(i)
	delete[]m_weights[i];
	END_FOREACH
	delete[]m_weights;
#endif // _DEBUG
}

void FXChessboard::SetPointWeight(const UINT16 & row, const UINT16 & col, const LONG & weight)
{
#ifdef _DEBUG
	if (CheckPoint(row, col))
		m_weights[row][col] = weight;
#endif // _DEBUG
}

void FXChessboard::ClearPointWeights()
{
#ifdef _DEBUG
	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	m_weights[i][j] = 0;
	END_FOREACH
	END_FOREACH
#endif // _DEBUG
}
void FXChessboard::PaintWeights(const HDC & dc, const POINT & topLeft, const LONG & unit)
{
#ifdef _DEBUG
	HFONT font = CreateFont(12, 6, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, _T("ÐÂËÎÌå"));
	HGDIOBJ obj = SelectObject(dc, font);

	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(255, 0, 0));

	BEGIN_FOREACH_ROW(i)
	BEGIN_FOREACH_COLUMN(j)
	if (m_weights[i][j])
	{
		POINT pos = { topLeft.x + unit * i, topLeft.y + unit * j };
		RECT rect = { pos.x, pos.y, pos.x + unit, pos.y + unit };
		TCHAR text[32];
		int c = _stprintf_s(text, 32, _T("%x"), m_weights[i][j]);
		DrawText(dc, text, c, &rect, 0);
	}
	END_FOREACH
	END_FOREACH

	SelectObject(dc, obj);
#endif // _DEBUG
}
