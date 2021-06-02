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

void gmInput(ChessBoard* board, HumanPlayer * p, char* str, Move * move, int turn) {
	char input[5];
	char temp[5];
	const char *del = " ";
	char *s = strtok(str, del);
	int i = 0;

	while(s != NULL) {
		if(i == 3)
		{
			memcpy(input, s, 2);
			input[2] = '\0';
		}
		else if(i == 5)
		{
			memcpy(temp, s, 2);
			temp[2] = '\0';
		}

		s = strtok(NULL, del);
		++i;
	}

	strcat(input, temp);
	
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
	char *buffer = (char *) malloc(100 * sizeof(char));

	// Initialize players
	HumanPlayer p1(WHITE);
	HumanPlayer p2(BLACK);

	// setup board
	board.initDefaultSetup();
	board.print();
	
	if(!(fp = fopen("Game4_LV3_Record.csv", "r")))
	{
		printf("File open failed!\n");
		return -1;
	}

	fgets(buffer, 100, fp);

	for(int i = 0; i < 5; ++i) {
		fgets(buffer, 100, fp);

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
