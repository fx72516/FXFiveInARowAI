#pragma once

class FXChessboard;

// typedef LONG(*FXAdjoinFunc)(const LONG & src, const LONG & weight, const UINT16 & link);
// typedef LONG(*FXSameTypeFunc)(const LONG & src, const LONG & weight, const UINT16 & space);
// typedef LONG(*FXEmptyFunc)(const LONG & src, const LONG & weight, const UINT16 & count);
// 
// struct FX_FIAR_AI
// {
// 	LONG AdjoinWeight;
// 	LONG SameTypeWeight;
// 	LONG EmptyWeight;
// 
// 	FXAdjoinFunc AdjoinFunc;
// 	FXSameTypeFunc SameTypeFunc;
// 	FXEmptyFunc EmptyFunc;
// };

typedef LONG(*FXCalcWeightInARow)(const FXChessboard * pChessboard, const std::vector<CHESS_POINT> & src, const UINT16 & current, const bool & isSelf);

class FXFIARPlayer
{
// 	LONG	m_adjoinWeight;
// 	LONG	m_sameTypeWeight;
// 	LONG	m_emptyWeight;
// 
// 	FXAdjoinFunc	m_adjoinFunc;
// 	FXSameTypeFunc	m_sameTypeFunc;
// 	FXEmptyFunc		m_emptyFunc;

	FXCalcWeightInARow m_calcWeightIAR;

	CHESS_TYPE m_chessType;

	FXChessboard * m_chessboard;

	bool Thinking(UINT16 & row, UINT16 & col) const;

	LONG CalcPointWeight(const UINT16 & row, const UINT16 & col) const;
	LONG CalcPointWeight(const UINT16 & row, const UINT16 & col, const CHESS_TYPE & type) const;

public:
	FXFIARPlayer();
	~FXFIARPlayer();

	CHESS_TYPE ChessType() const;
	void SetChessType(CHESS_TYPE chessType);

// 	void SetAI(const FX_FIAR_AI & ai);

	bool Turn(FXChessboard * chessboard);

#ifdef TEST
	bool Test(FXChessboard * chessboard, const CHESS_TYPE & type);
	bool Test(FXChessboard * chessboard, const UINT16 & row, const UINT16 & col, const CHESS_TYPE & type);
#endif
};

