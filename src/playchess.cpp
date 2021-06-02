#include <cstdlib>
#include <cstdio>
#include <list>
#include "chessboard.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "CycleTimer.h"
#include <omp.h>
#include <cstring>
#include <string>

using namespace std;

void startAutoChess() {
	ChessBoard board;
	list<Move> regulars, nulls;
	int turn = WHITE;
	Move move;

	// Initialize players
	HumanPlayer p1(WHITE);
	HumanPlayer p2(BLACK);
	
	FILE *fp;
	int buffersize = 10;
	char *buffer = (char *) malloc(buffersize * sizeof(char));
	
	if(!(fp = fopen("Game4_LV3_RecordPosition.csv", "r")))
	{
		printf("File open failed!\n");
		return -1;
	}
	
	// setup board
	board.initDefaultSetup();
	board.print();
	
	for(int i = 0; i < 67; ++i) {
		// query player's choice
		if(!p->processInput(input, *move)) {
			printf("Error while parsing input.\n");
			return;
		}

		if(!board->isValidMove(turn, *move)) {
			printf("Invalid move.\n");
			return;
		}

		// if player has a move get all moves
		regulars.clear();
		nulls.clear();
		board.getMoves(turn, regulars, regulars, nulls);

		// execute maintenance moves
		for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
			board.move(*it);

		// execute move
		board.move(move);
		//move.print(mesg);

		// opponents turn
		turn = TOGGLE_COLOR(turn);
		board.print();
	}
	
	fclose(fp);
	free(buffer);
}

int main() {
	startAutoChess();
	return 0;
}
