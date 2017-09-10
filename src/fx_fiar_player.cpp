#include "stdafx.h"
#include "fx_fiar_player.h"
#include "fx_chessboard.h"
#include "fx_chessman.h"
#include "fx_application.h"



// static LONG DefaultAdjoinFunc(const LONG & src, const LONG & weight, const UINT16 & link)
// {
// 	return src + (LONG)pow((weight + 1), link);
// }
// 
// static LONG DefaultSameTypeFunc(const LONG & src, const LONG & weight, const UINT16 & space)
// {
// 	return src + (weight + 1) / (space);
// }
// 
// static LONG DefaultEmptyFunc(const LONG & src, const LONG & weight, const UINT16 & count)
// {
// 	return src + weight / (count + 1);
// }
// 
// const static FX_FIAR_AI DefaultAI = { 10, 10, 10, DefaultAdjoinFunc, DefaultSameTypeFunc, DefaultEmptyFunc };

const static LONG WEIGHT_VALUES[] = {
	0xfffff,
	0xffff,
	0xfff,
	0xff,
	0xf,
	1,
};
const static UINT16 WEIGHT_VALUES_COUNT = sizeof(WEIGHT_VALUES) / sizeof(WEIGHT_VALUES[0]);

// const static LONG WEIGHT_VALUES_HALF[] = {
// 	0xfff,
// 	0xff,
// 	0xf,
// 	0,
// };

static LONG DefaultCalcWeightIAR(
	const FXChessboard * pChessboard, 
	const std::vector<CHESS_POINT> & pointList, 
	const UINT16 & current, 
	const bool & isSelf)
{
	ASSERT(pChessboard);
	ASSERT(!(*pChessboard)[pointList[current]]);
	if (pointList.size() < FIVE)
		return 1;

	bool firstIsSpace = !(0 == current || (*pChessboard)[pointList[0]]);
	bool lastIsSpace = !(pointList.size() - 1 == current || (*pChessboard)[pointList[pointList.size() - 1]]);
	std::vector<UINT16> link;
	std::vector<UINT16> space;
	link.push_back(0);
	space.push_back(0);
	for (UINT16 i = 0; i < pointList.size(); ++i)
	{
		if (i == current || (*pChessboard)[pointList[i]])
		{
			if (*(space.end() - 1))
				space.push_back(0);
			++(*(link.end() - 1));
		}
		else
		{
			if (*(link.end() - 1))
				link.push_back(0);
			++(*(space.end() - 1));
		}
	}
	if (*(link.end() - 1) == 0)
	{
		link.erase(link.end() - 1);
		space.erase(space.end() - 1);
	}
	ASSERT(abs((INT16)(link.size() - space.size())) <= 1);

	LONG maxWeight = 0;
	for (UINT16 i = 0; i < link.size(); ++i)
	{
		LONG weight = 0;
		UINT16 totalCount = 0;
		UINT16 spaceCount = 0;
		for (UINT16 j = i; j < link.size(); ++j)
		{
			LONG _weight = 0;
			totalCount += link[j];
// 			if (totalCount >= FIVE)
// 			{
// 				weight = max(spaceCount < WEIGHT_VALUES_COUNT ? WEIGHT_VALUES[spaceCount] : 0, weight) + (isSelf ? 1 : 0);
// 			}
// 			else
			{
				UINT16 space = spaceCount + max(FIVE - totalCount, 0);
				_weight = space < WEIGHT_VALUES_COUNT ? WEIGHT_VALUES[space] : 0;
				if ((totalCount >= FIVE && space == 0) || (totalCount == 4 && space == 0 && firstIsSpace && lastIsSpace))
					_weight += WEIGHT_VALUES[WEIGHT_VALUES_COUNT - (isSelf ? 4 : 3) + space];
				else if (firstIsSpace && lastIsSpace)
					_weight += WEIGHT_VALUES[WEIGHT_VALUES_COUNT - (*std::max_element(link.cbegin(), link.cend()))];
				else if (!isSelf)
					_weight += WEIGHT_VALUES[WEIGHT_VALUES_COUNT - 1];

				_weight += (*std::max_element(link.cbegin(), link.cend())) * WEIGHT_VALUES[WEIGHT_VALUES_COUNT - (isSelf ? 1 : 2)];
			}

			UINT16 nextSpaceIdx = firstIsSpace ? j + 1 : j;
			if (space.size() > nextSpaceIdx)
			{
				totalCount += space[nextSpaceIdx];
				spaceCount += space[nextSpaceIdx];
			}
			else
			{
				ASSERT(j + 1 == link.size());
			}

			weight = max(_weight, weight);
		}
		maxWeight = max(weight, maxWeight);
	}

	if (pChessboard->IsEmptyAll(&pointList))
		maxWeight += pointList.size() * WEIGHT_VALUES[WEIGHT_VALUES_COUNT - 1];
	return maxWeight;
}

FXFIARPlayer::FXFIARPlayer()
{
// 	SetAI(DefaultAI);
	m_calcWeightIAR = DefaultCalcWeightIAR;
}

FXFIARPlayer::~FXFIARPlayer()
{
}

CHESS_TYPE FXFIARPlayer::ChessType() const
{
	return m_chessType;
}

void FXFIARPlayer::SetChessType(CHESS_TYPE chessType)
{
	m_chessType = chessType;
}

// void FXFIARPlayer::SetAI(const FX_FIAR_AI & ai)
// {
// 	m_adjoinWeight		= ai.AdjoinWeight;
// 	m_sameTypeWeight	= ai.SameTypeWeight;
// 	m_emptyWeight		= ai.EmptyWeight;
// 	m_adjoinFunc		= ai.AdjoinFunc;
// 	m_sameTypeFunc		= ai.SameTypeFunc;
// 	m_emptyFunc			= ai.EmptyFunc;
// }

bool FXFIARPlayer::Turn(FXChessboard * chessboard)
{
	m_chessboard = chessboard;

	UINT16 row = -1;
	UINT16 col = -1;
	CHECK_RET_FALSE(Thinking(row, col));
	return FXApplication::Instance()->MoveChessman(row, col);
}

#ifdef TEST
bool FXFIARPlayer::Test(FXChessboard * chessboard, const CHESS_TYPE & type)
{
	m_chessboard = chessboard;
	SetChessType(type);

	UINT16 row = -1;
	UINT16 col = -1;
	return Thinking(row, col);
}

bool FXFIARPlayer::Test(FXChessboard * chessboard, const UINT16 & row, const UINT16 & col, const CHESS_TYPE & type)
{
	m_chessboard = chessboard;
	SetChessType(type);

	CalcPointWeight(row, col);

	return true;
}
#endif

bool FXFIARPlayer::Thinking(UINT16 & row, UINT16 & col) const
{
	if (m_chessboard->IsOver())
		return false;

	LONG maxWeight = 0;
	std::vector<CHESS_POINT> maxWeightPoints;
	for (UINT16 i = 0; i < m_chessboard->rowCount(); ++i)
	{
		for (UINT16 j = 0; j < m_chessboard->colCount(); ++j)
		{
			LONG weight = CalcPointWeight(i, j);
			if (weight >= maxWeight)
			{
				if (weight > maxWeight)
				{
					maxWeight = weight;
					maxWeightPoints.clear();
				}
				maxWeightPoints.push_back(CHESS_POINT(i, j));
			}
		}
	}
	ASSERT(maxWeight > 0);

	if (maxWeightPoints.size() > 0)
	{
		CHESS_POINT randPoint = maxWeightPoints[(FXApplication::Random(0, maxWeightPoints.size()))];
		row = randPoint.row;
		col = randPoint.col;
		return true;
	}
	return false;
}

LONG FXFIARPlayer::CalcPointWeight(const UINT16 & row, const UINT16 & col) const
{
#ifdef TEST
	LONG weight = CalcPointWeight(row, col, m_chessType) + CalcPointWeight(row, col, !m_chessType);
	m_chessboard->SetPointWeight(row, col, weight);
	return weight;
#else
	return CalcPointWeight(row, col, m_chessType) + CalcPointWeight(row, col, !m_chessType);
#endif
}

LONG FXFIARPlayer::CalcPointWeight(const UINT16 & row, const UINT16 & col, const CHESS_TYPE & type) const
{
	CHECK_ASSERT_RET_FALSE(m_chessboard->CheckPoint(row, col));
	if (m_chessboard->GetChessman(row, col))
		return 0;

	auto CheckFiveInARow = [&](INT16 dirRow, INT16 dirCol)
	{
// 		LONG weight = 1;
// 		UINT16 link = 1;
// 		UINT16 mayLink = 1;
		UINT16 current = UNUSEFULL;
		std::vector<CHESS_POINT> customList;
		INT16 dir[] = { -1, 1 };
		for (INT16 d = 0; d < 2; ++d)
		{
			UINT16 space = 0;
			UINT16 count = 0;
			for (INT16 i = 1; i < FIVE; ++i)
			{
				INT16 r = (INT16)row + i * dirRow * dir[d];
				INT16 c = (INT16)col + i * dirCol * dir[d];
				if (r < 0 || r >= m_chessboard->rowCount() || c < 0 || c >= m_chessboard->colCount())
					break;

				FXChessman * pChessman = m_chessboard->GetChessman(r, c);
				if (pChessman && pChessman->Type() != type)
					break;

				if (d == 0)
					customList.insert(customList.cbegin(), CHESS_POINT(r, c));
				else
					customList.push_back(CHESS_POINT(r, c));

// 				FXChessman * pChessman = m_chessboard->GetChessman(r, c);
// 				if (!pChessman)
// 					++space, weight = m_emptyFunc(weight, m_emptyWeight, count++);
// 				else if (pChessman->Type() != type)
// 					break;
// 				else if (space == 0)
// 					++link, weight = m_adjoinFunc(weight, m_adjoinWeight, link);
// 				else
// 					weight = m_sameTypeFunc(weight, m_sameTypeWeight, space);
// 				++mayLink;
			}

			if (d == 0)
			{
				current = (UINT16)customList.size();
				customList.push_back(CHESS_POINT(row, col));
			}
		}

		return max(m_calcWeightIAR(m_chessboard, customList, current, type == m_chessType), 1);
	};

	LONG weight = 0;
	INT16 dir[][2] = { { 1, 0 },{ 0, 1 },{ 1, 1 },{ 1, -1 } };
	for (UINT16 i = 0; i < 4; ++i)
		weight += CheckFiveInARow(dir[i][0], dir[i][1]);

	return weight;

}
