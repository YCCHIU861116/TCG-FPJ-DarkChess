/*! \file eopXD.h
    \brief header file for searching agent of CDC
    \author Yueh-Ting Chen, modified from project held by Jr-Chang Chen
    \course Theory of Computer Game (TCG)
*/

#ifndef CDC
#define CDC
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>

#define RED 0
#define BLACK 1
#define CHESS_COVER -1
#define CHESS_EMPTY -2
#define COMMAND_NUM 18
class CDCagent 
{
enum { 
	EMPTY=0,
	RK, RG, RM, RR, RN, RC, RP,
	CLOSE,
	BK, BG, BM, BR, BN, BC, BP,
	NUM_KIND
};
const char* commands_name[COMMAND_NUM] = {
	"protocol_version",
	"name",
	"version",
	"known_command",
	"list_commands",
	"quit",
	"boardsize",
	"reset_board",
	"num_repetition",
	"num_moves_to_draw",
	"move",
	"flip",
	"genmove",
	"game_over",
	"ready",
	"time_settings",
	"time_left",
	"showboard"
};

// Constructor / Destructor
public:
	CDCagent(void);
	~CDCagent(void);
// Commands from DCTP
public:
	bool protocol_version(const char* data[], char* response);// 0
	bool name(const char* data[], char* response);// 1
	bool version(const char* data[], char* response);// 2
	bool known_command(const char* data[], char* response);// 3
	bool list_commands(const char* data[], char* response);// 4
	bool quit(const char* data[], char* response);// 5
	bool boardsize(const char* data[], char* response);// 6
	bool reset_board(const char* data[], char* response);// 7
	bool num_repetition(const char* data[], char* response);// 8
	bool num_moves_to_draw(const char* data[], char* response);// 9
	bool move(const char* data[], char* response);// 10
	bool flip(const char* data[], char* response);// 11
	bool genmove(const char* data[], char* response);// 12
	bool game_over(const char* data[], char* response);// 13
	bool ready(const char* data[], char* response);// 14
	bool time_settings(const char* data[], char* response);// 15
	bool time_left(const char* data[], char* response);// 16
	bool showboard(const char* data[], char* response);// 17
// Utility Provided
public:
	int ConvertChessNo(int input);
	void initBoardState ();
	int FlipList ( int *flip_moves );
	int NonFlipList ( int* Board,int color,int *non_flip_moves );
	bool IsLegal ( int* chess,int from_location_no,int to_location_no,int UserId );
	void Print_Chessboard ();
	void Print_Chess ( int chess_no,char *Result );
	void Play(char move[6]);
	void MakeMove(const char move[6]);
private:
	int Color;
	int Red_Time, Black_Time;
	int Board[32];
	int CloseChess[14];
};


#endif