/*! \file agent.cpp
    \brief implementation for searching agent of CDC
    \author Yueh-Ting Chen, modified from project held by Jr-Chang Chen
    \course Theory of Computer Game (TCG)
*/

#include "agent.h"

CDCagent::CDCagent ( void ) { this->Color = 2; }
CDCagent::~CDCagent ( void ) {}

//-----------------------------------------------DCTP protocol------------------------------------------------
bool CDCagent::protocol_version ( const char* data[], char* response ) {
	strcpy(response, "1.0.0");
	return 0;
}
bool CDCagent::name ( const char* data[], char* response ) {
	strcpy(response, "eopXD");
	return 0;
}
bool CDCagent::version ( const char* data[], char* response ) {
	strcpy(response, "0.0.1");
	return 0;
}
bool CDCagent::known_command ( const char* data[], char* response ) {
	for(int i = 0; i < COMMAND_NUM; i++){
		if(!strcmp(data[0], commands_name[i])){
			strcpy(response, "true");
			return 0;
		}
	}
	strcpy(response, "false");
	return 0;
}
bool CDCagent::list_commands ( const char* data[], char* response ) {
	for(int i = 0; i < COMMAND_NUM; i++){
		strcat(response, commands_name[i]);
		if(i < COMMAND_NUM - 1){
			strcat(response, "\n");
		}
	}
	return 0;
}
bool CDCagent::quit(const char* data[], char* response){
	fprintf(stderr, "End Of Program\n"); 
	return 0;
}
bool CDCagent::boardsize ( const char* data[], char* response ) {
	fprintf(stderr, "BoardSize: %s x %s\n", data[0], data[1]); 
	return 0;
}
bool CDCagent::reset_board ( const char* data[], char* response ) {
	this->Color = 2;
	this->Red_Time = -1; // known
	this->Black_Time = -1; // known
	this->initBoardState();
	return 0;
}
bool CDCagent::num_repetition ( const char* data[], char* response ) { return 0; }
bool CDCagent::num_moves_to_draw ( const char* data[], char* response ) { return 0; }
bool CDCagent::move ( const char* data[], char* response ) {
  char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}
bool CDCagent::flip(const char* data[], char* response){
	char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(move);
	return 0;
}
bool CDCagent::genmove ( const char* data[], char* response ) {
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = RED;
	}else if(!strcmp(data[0], "black")){
		this->Color = BLACK;
	}else{
		this->Color = 2;
	}
	// genmove
	char move[6];
	this->Play(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	return 0;
}
bool CDCagent::game_over ( const char* data[], char* response ) {
  	fprintf(stderr, "Game Result: %s\n", data[0]); 
	return 0;
}
bool CDCagent::ready ( const char* data[], char* response ) { return 0; }
bool CDCagent::time_settings ( const char* data[], char* response ) { return 0; }
bool CDCagent::time_left ( const char* data[], char* response ) {
	if(!strcmp(data[0], "red")){
		sscanf(data[1], "%d", &(this->Red_Time));
	} else{
		sscanf(data[1], "%d", &(this->Black_Time));
	}
	fprintf(stderr, "Time Left(%s): %s\n", data[0], data[1]); 
	return 0;
}
bool CDCagent::showboard ( const char* data[], char* response){
	Print_Chessboard();
	return 0;
}
//----------------------------------------------utility provided----------------------------------------------
int GetFin(char c) {
	static const char skind[]={'-','K','G','M','R','N','C','P','X','k','g','m','r','n','c','p'};
	for(int f=0;f<16;f++){ 
		if(c==skind[f]) {
			return f;
		}
	}
	return -1;
}
int CDCagent::ConvertChessNo(int input) { // into self-define id for IsLegal
	switch(input) {
		case 0: return CHESS_EMPTY; break; case 8: return CHESS_COVER; break;
		case 1: return   6; break; case 2: return   5; break;
		case 3: return   4; break; case 4: return   3; break;
		case 5: return   2; break; case 6: return   1; break;
		case 7: return   0; break; case 9: return  13; break;
		case 10: return 12; break; case 11: return 11; break;
		case 12: return 10; break; case 13: return 9; break;
		case 14: return 8; break; case 15: return 7; break;
	}
	return -1;
}
const char pieceChar[20][5] = { 
	"-",
	"K","G","M","R","N","C","P",
	"X",
	"k","g","m","r","n","c","p"
};
void CDCagent::initBoardState () {
	// initial board
	char iCurrentPosition[32];
	for(int i = 0; i < 32; i++){ iCurrentPosition[i] = 'X'; }
	int iPieceCount[14] = {1, 2, 2, 2, 2, 2, 5, 1, 2, 2, 2, 2, 2, 5};

	fprintf(stderr, "\niPieceCount[14]:\n");
	for(int i=0;i<14;i++) {
		fprintf(stderr, "[ %s ]",pieceChar[i]); 
	} fprintf(stderr, "\n");
	for(int i=0;i<14;i++) {
		printf("%4d",iPieceCount[i]); 
	} fprintf(stderr, "\n");
	puts("iCurrentPosition[32]:");
	for(int i=28;i>=0;i-=4){
		for(int j=0;j<4;j++) {
			fprintf(stderr, "%2c ",iCurrentPosition[i+j]);
		}
		fprintf(stderr, " | ");
		for(int j=0;j<4;j++) { 
			fprintf(stderr, "%2d ",i+j);
		}
		if(i%4==0) {
			fprintf(stderr, "\n");
		}
	} fprintf(stderr, "\n"); 
	fprintf(stderr, "\n\n\n");
	memcpy(this->CloseChess,iPieceCount,sizeof(int)*14);
	//convert to my format
	int Index = 0;
	for(int i=0;i<8;i++) { for(int j=0;j<4;j++) {
		this->Board[Index] = ConvertChessNo(GetFin(iCurrentPosition[Index]));
		Index++;
	}}
	Print_Chessboard();
}
// apply move to the board 
void CDCagent::MakeMove(const char move[6]) { 
	int src, dst;
	src = ('8'-move[1])*4+(move[0]-'a');
	if(move[2]=='('){ 
		fprintf(stderr, "FLIP: %c%c = %s\n",move[0], move[1], pieceChar[GetFin(move[3])]); 
		this->Board[src] = ConvertChessNo(GetFin(move[3]));
		Print_Chessboard();
	}else { 
		dst = ('8'-move[4])*4+(move[3]-'a');
		fprintf(stderr, "MOVE: %c%c - %c%c\n",move[0], move[1], move[3], move[4]); 
		this->Board[dst] = this->Board[src];
		this->Board[src] = CHESS_EMPTY;
		Print_Chessboard();
	}
}
//------------------------------------------ legal move generation -------------------------------------------
int CDCagent::FlipList ( int *flip_moves ) {
	int res = 0;
	for(int i=0;i<32;i++) {
		if(this->Board[i] == CHESS_COVER) {
			flip_moves[res++] = i*100+i;
		}
	}
	return (res);
}
int CDCagent::NonFlipList ( int* Board,int color,int *non_flip_moves ) {
	int res = 0;
	for(int i=0;i<32;i++) {
		if ( Board[i] >= 0 && Board[i]/7 == color ) {
			if ( Board[i] % 7 == 1 ) { // Cannon
				int row = i/4;
				int col = i%4;
				for ( int rowCount=row*4; rowCount<(row+1)*4; rowCount++ ) {
					if ( IsLegal(Board,i,rowCount,color)) {
						non_flip_moves[res++] = i*100+rowCount;
					}
				}
				for(int colCount=col; colCount<32;colCount += 4) {
					if(IsLegal(Board,i,colCount,color)) {
						non_flip_moves[res++] = i*100+colCount;
					}
				}
			}
			else {
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++) {
					if(Move[k] >= 0 && Move[k] < 32 && IsLegal(Board,i,Move[k],color)) {
						non_flip_moves[res++] = i*100+Move[k];
					}
				}
			}
		};
	}
	return (res);
}
// judge if the move is legal
bool CDCagent::IsLegal ( int* chess,int from_location_no,int to_location_no,int UserId ) {
	bool IsCurrent = true;
	int from_chess_no = chess[from_location_no], to_chess_no = chess[to_location_no];
	int from_row = from_location_no / 4, to_row = to_location_no / 4;
	int from_col = from_location_no % 4, to_col = to_location_no % 4;
	if(from_chess_no < 0 || ( to_chess_no < 0 && to_chess_no != CHESS_EMPTY) ) {  
		IsCurrent = false;
	}
	else if (from_chess_no >= 0 && from_chess_no / 7 != UserId) {
		IsCurrent = false;
	}
	else if((from_chess_no / 7 == to_chess_no / 7) && to_chess_no >= 0) {
		IsCurrent = false;
	}
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1){ //legal move 
		IsCurrent = true;
	}	
	else if(from_chess_no % 7 == 1 ) { //judge cannon
		int row_gap = from_row-to_row;
		int col_gap = from_col-to_col;
		int between_Count = 0;
		if(from_row-to_row == 0 || from_col-to_col == 0) {
			if(row_gap == 0)  {
				for(int i=1;i<abs(col_gap);i++) {
					int between_chess;
					if(col_gap>0) { between_chess = chess[from_location_no-i] ; }
					else { between_chess = chess[from_location_no+i] ; }
					if(between_chess != CHESS_EMPTY) { between_Count++; }
				}
			}
			else {
				for(int i=1;i<abs(row_gap);i++) {
					int between_chess;
					if(row_gap > 0) { between_chess = chess[from_location_no-4*i] ; }
					else { between_chess = chess[from_location_no+4*i] ; }
					if(between_chess != CHESS_EMPTY) { between_Count++; }
				}
			}
			if(between_Count != 1 ) {
				IsCurrent = false;
			}
			else if(to_chess_no == CHESS_EMPTY) {
				IsCurrent = false;
			}
		}
		else {
			IsCurrent = false;
		}
	}
	else { // non cannon
		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1) {
			IsCurrent = false;
		}
		else if(from_chess_no % 7 == 0) {
			if(to_chess_no % 7 != 0 && to_chess_no % 7 != 6) {
				IsCurrent = false;
			}
		}
		else if(from_chess_no % 7 == 6 && to_chess_no % 7 == 0) {
			IsCurrent = false;
		}
		else if(from_chess_no % 7 < to_chess_no% 7) {
			IsCurrent = false;
		}
	}
	return IsCurrent;
}

//-------------------------------------------- Display -------------------------------------------------------
//Display chess board
void CDCagent::Print_Chessboard () {	
	char Mes[1024]="";
	char temp[1024];
	char myColor[10]="";
	if(Color == -99) {
		strcpy(myColor,"Unknown");
	}
	else if(this->Color == RED) {
		strcpy(myColor,"Red");
	}
	else {
		strcpy(myColor,"Black");
	}
	sprintf(temp,"------------%s-------------\n",myColor);
	strcat(Mes,temp);
	strcat(Mes,"<8> ");
	for(int i=0;i<32;i++){
		if(i != 0 && i % 4 == 0){
			sprintf(temp,"\n<%d> ",8-(i/4));
			strcat(Mes,temp);
		}
		char chess_name[10]="";
		Print_Chess(this->Board[i],chess_name);
		sprintf(temp,"%5s", chess_name);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n     ");
	for(int i=0;i<4;i++){
		sprintf(temp," <%c> ",'a'+i);
		strcat(Mes,temp);
	}
	strcat(Mes,"\n\n");
	fprintf(stderr, "%s",Mes);
}
//Print chess
void CDCagent::Print_Chess ( int chess_no,char *Result ) {
	// XX -> Empty
	if(chess_no == CHESS_EMPTY) { strcat(Result, " - "); return; }
	//OO -> DarkChess
	else if(chess_no == CHESS_COVER) { strcat(Result, " X "); return; }
	
	switch ( chess_no ) {
		case  0: strcat(Result, "[P]"); break; case  1: strcat(Result, "[C]"); break;
		case  2: strcat(Result, "[N]"); break; case  3: strcat(Result, "[R]"); break;
		case  4: strcat(Result, "[M]"); break; case  5: strcat(Result, "[G]"); break;
		case  6: strcat(Result, "[K]"); break; case  7: strcat(Result, "{p}"); break;
		case  8: strcat(Result, "{c}"); break; case  9: strcat(Result, "{n}"); break;
		case 10: strcat(Result, "{r}"); break; case 11: strcat(Result, "{m}"); break;
		case 12: strcat(Result, "{g}"); break; case 13: strcat(Result, "{k}"); break;
	}
}
//-------------------------------------------- playing function ----------------------------------------------
void CDCagent::Play(char move[6]) {
	// move generation	
	int non_flip_moves[100];
	int total_non_flip = this->NonFlipList(this->Board, this->Color, non_flip_moves);
	int flip_moves[32];
	int total_flip = this->FlipList(flip_moves);
	//fprintf(stderr, "non_flip: %d, flip: %d\n", total_non_flip, total_flip);

	// move decision (searching happens here)
	int Answer = 0;
	
	#ifdef RANDOM // random decision on move
	// if able to do non-flip move
	if ( total_non_flip > 0 ) {
		Answer = non_flip_moves[rand()%total_non_flip];
	} else if ( total_flip > 0 ) {
		Answer = flip_moves[rand()%total_flip];
	} else {
		fprintf(stderr, "ERROR: no legal move\n");
		exit(1);
	}
	#endif

	// move translation 
	int startPoint = Answer/100;
	int endPoint   = Answer%100;
	sprintf(move, "%c%c-%c%c",'a'+(startPoint%4),'1'+(7-startPoint/4),'a'+(endPoint%4),'1'+(7-endPoint/4));
	
	char chess_Start[10] = "", chess_End[10] = "";
	Print_Chess(Board[startPoint],chess_Start);
	Print_Chess(Board[endPoint],chess_End);
	
	fprintf(stderr, "My move:\n");
	fprintf(stderr, "<%s> -> <%s>\n",chess_Start,chess_End);
	fprintf(stderr, "move:%s\n",move);
	this->Print_Chessboard();
}
