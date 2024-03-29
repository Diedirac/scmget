#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <list>
#include <vector>
#include "aiplayer.h"
#include "chessboard.h"
#include <omp.h>

using namespace std;

AIPlayer::AIPlayer(int color, int search_depth)
 : ChessPlayer(color),
   search_depth(search_depth)
{
	srand(8888);
}

AIPlayer::~AIPlayer()
{}

bool AIPlayer::getMove(ChessBoard & board, Move & move) const
{
	list<Move> regulars, nulls;
	vector<Move> candidates;
	bool quiescent = false;
	int best, tmp;

	// first assume we are loosing
	best = -KING_VALUE;

	// get all moves
	board.getMoves(this->color, regulars, regulars, nulls);

	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);

	// loop over all moves
	
	#pragma omp parallel
	{
		#pragma omp for
		for(list<Move>::iterator it = regulars.begin(); it != regulars.end(); ++it)
		{
			// execute move
			board.move(*it);

			// check if own king is vulnerable now
			if(!board.isVulnerable((this->color ? board.black_king_pos : board.white_king_pos), this->color)) {

				if((*it).capture != EMPTY) {
					quiescent = true;
				}

				// recursion
				tmp = -evalAlphaBeta(board, TOGGLE_COLOR(this->color), this->search_depth - 1, -WIN_VALUE, -best, quiescent);
				if(tmp > best) {
					best = tmp;
					candidates.clear();
					candidates.push_back(*it);
				}
				else if(tmp == best) {
					candidates.push_back(*it);
				}
			}

			// undo move and inc iterator
			board.undoMove(*it);
		}
	}

	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);

	// loosing the game?
	if(best < -WIN_VALUE) {
		return false;
	}
	else {
		// select random move from candidate moves
		move = candidates[rand() % candidates.size()];
		return true;
	}
}

bool AIPlayer::getMove(ChessBoard & board, Move & move, unsigned long long* nodeCount) const
{
	list<Move> regulars, nulls;
	vector<Move> candidates;
    bool quiescent = false;
	int best, tmp;

	// first assume we are loosing
	best = -KING_VALUE;

	// get all moves
	board.getMoves(this->color, regulars, regulars, nulls);

	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);

	// loop over all moves
	for(list<Move>::iterator it = regulars.begin(); it != regulars.end(); ++it)
	{
		// execute move
		board.move(*it);

		// check if own king is vulnerable now
		if(!board.isVulnerable((this->color ? board.black_king_pos : board.white_king_pos), this->color)) {

			if((*it).capture != EMPTY) {
				quiescent = true;
			}

			// recursion
			tmp = -evalAlphaBeta(board, TOGGLE_COLOR(this->color), this->search_depth - 1, -WIN_VALUE, -best, quiescent, nodeCount);
			if(tmp > best) {
				best = tmp;
				candidates.clear();
				candidates.push_back(*it);
			}
			else if(tmp == best) {
				candidates.push_back(*it);
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
	}

	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);

	// loosing the game?
	if(best < -WIN_VALUE) {
		return false;
	}
	else {
		// select random move from candidate moves
		move = candidates[rand() % candidates.size()];
		return true;
	}
}

int AIPlayer::evalAlphaBeta(ChessBoard & board, int color, int search_depth, int alpha, int beta, bool quiescent) const
{
	list<Move> regulars, nulls;
	int best, tmp;

	if(search_depth <= 0 && !quiescent) {
		if(color)
			return -evaluateBoard(board);
		else
			return +evaluateBoard(board);
	}

	// first assume we are loosing
	best = -WIN_VALUE;

	// get all moves
	board.getMoves(color, regulars, regulars, nulls);
	
	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);
	
	// loop over all moves
	for(list<Move>::iterator it = regulars.begin();
		alpha <= beta && it != regulars.end(); ++it)
	{
		// execute move
		board.move(*it);

		// check if own king is vulnerable now
		if(!board.isVulnerable((color ? board.black_king_pos : board.white_king_pos), color)) {

			if((*it).capture == EMPTY)
				quiescent = false;
            else
                quiescent = true;

			// recursion 'n' pruning
			tmp = -evalAlphaBeta(board, TOGGLE_COLOR(color), search_depth - 1, -beta, -alpha, quiescent);
			if(tmp > best) {
				best = tmp;
				if(tmp > alpha) {
					alpha = tmp;
				}
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
	}
	
	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);
	
	return best;
}

int AIPlayer::evalAlphaBeta(ChessBoard & board, int color, int search_depth, int alpha, int beta, bool quiescent, unsigned long long* nodeCount) const
{
	list<Move> regulars, nulls;
	int best, tmp;

	if(search_depth <= 0 && !quiescent) {
		++*nodeCount;
		
		if(color)
			return -evaluateBoard(board);
		else
			return +evaluateBoard(board);
	}

	// first assume we are loosing
	best = -WIN_VALUE;

	// get all moves
	board.getMoves(color, regulars, regulars, nulls);
	
	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);
	
	// loop over all moves
	for(list<Move>::iterator it = regulars.begin();
		alpha <= beta && it != regulars.end(); ++it)
	{
		// execute move
		board.move(*it);

		// check if own king is vulnerable now
		if(!board.isVulnerable((color ? board.black_king_pos : board.white_king_pos), color)) {

			if((*it).capture == EMPTY)
				quiescent = false;
            else
                quiescent = true;

			// recursion 'n' pruning
			tmp = -evalAlphaBeta(board, TOGGLE_COLOR(color), search_depth - 1, -beta, -alpha, quiescent, nodeCount);
			if(tmp > best) {
				best = tmp;
				if(tmp > alpha) {
					alpha = tmp;
				}
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
	}
	
	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);
	
	return best;
}

int AIPlayer::evaluateBoard(const ChessBoard & board) const
{
	int sum = 0;
	int pos, figure, summand;

	for(pos = 0; pos < 64; pos++)
	{
		figure = board.square[pos];
		switch(FIGURE(figure))
		{
			case PAWN:
				summand = PAWN_VALUE;
				break;
			case ROOK:
				summand = ROOK_VALUE;
				break;
			case KNIGHT:
				summand = KNIGHT_VALUE;
				break;
			case BISHOP:
				summand = BISHOP_VALUE;
				break;
			case QUEEN:
				summand = QUEEN_VALUE;
				break;
			case KING:
				summand = KING_VALUE;
				break;
			default:
				summand = 0;
				break;
		}
		
		sum += IS_BLACK(figure) ? -summand : summand;
	}
	
	return sum;
}

int AIPlayer::evaluateBoardThread(const ChessBoard & board) const
{
	int sum = 0;
	int pos, figure, summand;

	#pragma omp parallel
	{
		#pragma omp for reduction(+:sum)
		for(pos = 0; pos < 64; pos++)
		{
			printf("This is %d\n", pos);
			figure = board.square[pos];
			switch(FIGURE(figure))
			{
				case PAWN:
					summand = PAWN_VALUE;
					break;
				case ROOK:
					summand = ROOK_VALUE;
					break;
				case KNIGHT:
					summand = KNIGHT_VALUE;
					break;
				case BISHOP:
					summand = BISHOP_VALUE;
					break;
				case QUEEN:
					summand = QUEEN_VALUE;
					break;
				case KING:
					summand = KING_VALUE;
					break;
				default:
					summand = 0;
					break;
			}
		
			sum += IS_BLACK(figure) ? -summand : summand;
		}
	}
	return sum;
}

