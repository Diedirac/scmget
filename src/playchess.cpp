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
	char buffer[100];


	char* pos = (char *) malloc(5 * sizeof(char));

	// Initialize players
	HumanPlayer p1(WHITE);
	HumanPlayer p2(BLACK);

	// setup board
	board.initDefaultSetup();
	fscanf(fp, "%s\n", buffer);
	board.print();
	
	if(!(fp = fopen("Game4_LV3_Record.csv", "r")))
	{
		printf("File open failed!\n");
		return -1;
	}

	for(int i = 0; i < 1; ++i) {
		// show board
		fscanf(fp, "%s,%s,\n", NULL, buffer);
		

		// query player's choice
		if(turn) {
			gmInput(&board, &p2, pos, &move, turn)
		}
		else {
			gmInput(&board, &p1, pos, &move, turn)
		}
	
		// opponents turn
		turn = TOGGLE_COLOR(turn);
		board.print();
	}
	
	return 0;
}
