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

void splitPos(char *str, char *pos) {
	char tempA[5];
	char tempB[5];
	const char *del = " ";
	char *s = strtok(str, del);
	int i = 0;

	while(s != NULL) {
		printf("%d: %s %llu\n", i, s, strlen(s));

		if(i == 3)
		{
			memcpy(tempA, s, 2);
			tempA[2] = '\0';
		}
		else if(i == 5)
		{
			memcpy(tempB, s, 2);
			tempB[2] = '\0';
		}

		s = strtok(NULL, del);
		++i;
	}

	strcat(tempA, tempB);
	printf("%s %llu\n", tempA, strlen(tempA));
	strcpy(pos, tempA);
}

int main() {
	ChessBoard board;
	int turn = WHITE;
	Move move;
	double start, decisionTime;
	FILE *fp;
	char *buffer = (char *) malloc(100 * sizeof(char));
	char *pos = (char *) malloc(5 * sizeof(char));

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
		printf("before split%d\n", i);
		splitPos(buffer, pos);
		printf(pos);

		// query player's choice
		if(turn) {
			gmInput(&board, &p2, pos, &move, turn);
		}
		else {
			gmInput(&board, &p1, pos, &move, turn);
		}
	
		// opponents turn
		turn = TOGGLE_COLOR(turn);

		// show board
		board.print();
	}

	fclose(fp);
	free(buffer);
	free(pos);
	return 0;
}
