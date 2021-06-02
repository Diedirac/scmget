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

void gmInput(ChessBoard* board, HumanPlayer * p, char* input, Move * move, int turn) {
	list<Move> regulars, nulls;

	if(!p->processInput(input, *move)) {
		printf("Error while parsing input.\n");
		return;
	}

	if(!board->isValidMove(turn, *move)) {
		printf("Invalid move.\n");
		return;
	}

	printf("\n");

	board->getMoves(turn, regulars, regulars, nulls);

	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board->move(*it);

	// execute move
	board->move(*move);
}

int main() {
	ChessBoard board;
	int turn = WHITE;
	Move move;
	double start, decisionTime;
	FILE *fp;
	int buffersize = 10;
	char *buffer = (char *) malloc(buffersize * sizeof(char));

	// Initialize players
	HumanPlayer p1(WHITE);
	HumanPlayer p2(BLACK);

	// setup board
	board.initDefaultSetup();
	board.print();

	if(!(fp = fopen("Game4_LV3_RecordPosition.csv", "r")))
	{
		printf("File open failed!\n");
		return -1;
	}

	for(int i = 0; i < 10; ++i) {
		fgets(buffer, buffersize, fp);

		printf("%s", buffer);

		// query player's choice
		if(turn) {
			gmInput(&board, &p2, buffer, &move, turn);
		}
		else {
			gmInput(&board, &p1, buffer, &move, turn);
		}
	
		// opponents turn
		turn = TOGGLE_COLOR(turn);

		// show board
		board.print();
	}

	fclose(fp);
	free(buffer);
	return 0;
}
