/*! \file agent.cpp
    \brief implementation for searching agent of CDC
    \author Yueh-Ting Chen, modified from project held by Jr-Chang Chen
    \course Theory of Computer Game (TCG)
*/

#include "agent.h"

CDCagent::CDCagent ( void ) { this->Color = 2; }
CDCagent::~CDCagent ( void ) {}

struct table_entry trans_table[2][HASH_TABLE_SIZE];
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
	this->plies = 0;
	this->initBoardState();
	this->hash_value = 0;
	for(int i = 0; i < 32; i++) this->hash_value ^= random_num[8*32+i];
	memset(trans_table[0],0,sizeof(table_entry)*HASH_TABLE_SIZE);
	memset(trans_table[1],0,sizeof(table_entry)*HASH_TABLE_SIZE);
	return 0;
}
bool CDCagent::num_repetition ( const char* data[], char* response ) { return 0; }
bool CDCagent::num_moves_to_draw ( const char* data[], char* response ) { return 0; }
bool CDCagent::move ( const char* data[], char* response ) {
  char move[6];
	sprintf(move, "%s-%s", data[0], data[1]);
	this->MakeMove(move);
	this->plies++;
	return 0;
}
bool CDCagent::flip(const char* data[], char* response){
	char move[6];
	sprintf(move, "%s(%s)", data[0], data[1]);
	this->MakeMove(move);
	this->plies++;
	return 0;
}
bool CDCagent::genmove ( const char* data[], char* response ) {
	// set color
	if(!strcmp(data[0], "red")){
		this->Color = RED;
		//if(this-plies == 0) this->hash_value ^= color_rand[this->Color];
	}else if(!strcmp(data[0], "black")){
		this->Color = BLACK;
		//if(this-plies == 0) this->hash_value ^= color_rand[this->Color];
	}else{
		this->Color = 2;
	}
	// genmove
	char move[6];
	//sprintf(response, "%s", "a1 a1");
	this->Play(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[3], move[4]);
	this->plies++;
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

const char chess_name[17] = "-KGMRNCPXkgmrncp";
int GetFin(char c) {
	for(int f=0;f<16;f++){ 
		if(c==chess_name[f]) {
			return f;
		}
	}
	return -1;
}

const int div4[32] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7};
const int mod4[32] = {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};
const int div8[32] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3};
const int mod8[32] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};

const int Chess_digit[14] = {1, 2, 2, 2, 2, 2, 3, 1, 2, 2, 2, 2, 2, 3};
const int cumulate_chessdigit[7] = {0,1,3,5,7,9,11};
const int Chess_fullnum[14] = {1, 2, 2, 2, 2, 2, 5, 1, 2, 2, 2, 2, 2, 5};
const int pos_value[32] = {0,1,1,0,1,2,2,1,2,3,3,2,3,4,4,3,3,4,4,3,2,3,3,2,1,2,2,1,0,1,1,0};
const int canmove[256] = 
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,
1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,
1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0
};
const int basic_chess_value[7] = {6000,5000,2500,1000,500,1000,300};
const int div4_basic[7] ={1500,1250,1125,250,125,250,75} ;
void CDCagent::initBoardState () {
	// initial board
	char iCurrentPosition[32];
	for(int i = 0; i < 32; i++){ iCurrentPosition[i] = 'X'; }	

	fprintf(stderr, "Chess_fullnum[14]:\n");
	for(int i=0;i<14;i++) {
		fprintf(stderr, "[ %c ]",chess_name[i]); 
	} fprintf(stderr, "\n");
	for(int i=0;i<14;i++) {
		fprintf(stderr,"%4d",Chess_fullnum[i]); 
	} fprintf(stderr, "\n");
	fprintf(stderr,"iCurrentPosition[32]:\n");
	puts("Let's start!");
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

	this->LiveChess = 190147925;//1011010101010110110101010101
	//convert to my format
	for(int i = 0; i < 4; i++){
		this->Board[i] = 0;
		for(int j = 0; j < 8; j++){
			this->Board[i] += (1 << (4*j+3));
		}
	}
	fprintf(stderr, "init:%u\n", this->Board[3]);
	Print_Chessboard();
}
// apply move to the board 
void CDCagent::MakeMove(const char move[6]) { 
	int src, dst;
	src = (move[1]-'1')*4+(move[0]-'a');
	int src_chess = locate(this->Board,src);
	//int opponent_color = this->Color? RED:BLACK;
	// this->hash_value ^= color_rand[this->Color];
	// this->hash_value ^= color_rand[opponent_color];
	//fprintf(stderr, "%d\n", src);
	if(move[2]=='('){ 
		fprintf(stderr, "FLIP: %c%c = %c\n",move[0], move[1], move[3]);
		int dst_chess = GetFin(move[3]); 
		modify(this->Board,src,dst_chess);
		this->hash_value ^= random_num[CHESS_COVER*32+src];
		this->hash_value ^= random_num[dst_chess*32+src];
		Print_Chessboard();
	}else { 
		dst = (move[4]-'1')*4+(move[3]-'a');
		fprintf(stderr, "MOVE: %c%c - %c%c\n",move[0], move[1], move[3], move[4]);
		int dst_chess = locate(this->Board,src); 
		if(dst_chess != CHESS_EMPTY){
			int dst_color = div8[dst_chess];
			int dst_kind = mod8[dst_chess]-1;
			this->LiveChess -= (1 << (dst_color*14+cumulate_chessdigit[dst_kind]));//low red high black
		}		
		this->hash_value ^= random_num[dst_chess*32+dst];
		this->hash_value ^= random_num[src_chess*32+dst];
		this->hash_value ^= random_num[src_chess*32+src];
		this->hash_value ^= random_num[CHESS_EMPTY*32+src];
		modify(this->Board,dst,src_chess);
		modify(this->Board,src,CHESS_EMPTY);
		Print_Chessboard();
	}
}
//------------------------------------------ legal move generation -------------------------------------------
int CDCagent::FlipList (unsigned  int* Board, int *flip_moves ) {
	int res = 0;
	for(int i=0;i<32;i++) {
		int a = locate(Board, i);
		//fprintf(stderr,"i = %d, a = %d\n",i,a);
		if(a == CHESS_COVER) {
			flip_moves[res++] = (i<<7)+i;
		}
	}
	return (res);
}
int CDCagent::NonFlipList (unsigned int* Board,int color,int *non_flip_moves ) {
	int res = 0;
	for(int i=0;i<32;i++) {
		if ( locate(Board,i) > 0 && div8[locate(Board,i)] == color ) {
			if ( mod8[locate(Board,i)] == 6 ) { // Cannon
				int row = div4[i], col = mod4[i];
				for ( int rowCount=row*4; rowCount<(row+1)*4; rowCount++ ) {
					if ( IsLegal(Board,i,rowCount,color)) {
						non_flip_moves[res++] = (i<<7)+rowCount;
						//fprintf(stderr, "from = %d(%d),to = %d\n", i,locate(Board,i),rowCount);
					}
				}
				for(int colCount=col; colCount<32;colCount += 4) {
					if(IsLegal(Board,i,colCount,color)) {
						non_flip_moves[res++] = (i<<7)+colCount;
						//fprintf(stderr, "from = %d(%d),to = %d\n", i,locate(Board,i),colCount);
					}
				}
			}
			else {
				int Move[4] = {i-4,i+1,i+4,i-1};
				for(int k=0; k<4;k++) {
					if(Move[k] >= 0 && Move[k] < 32 && IsLegal(Board,i,Move[k],color)) {
						non_flip_moves[res++] = (i<<7)+Move[k];
						//fprintf(stderr, "from = %d(%d),to = %d\n", i,locate(Board,i),Move[k]);
					}
				}
			}
		};
	}
	return (res);
}
int CDCagent::locate(unsigned int* Board, int index){//Board[num]]
	return (Board[div8[index]] >> (mod8[index]*4)) & 15;
}
void CDCagent::modify(unsigned int* Board, int index, int value){
	//fprintf(stderr, "modify:%u\n", (mod8[index]*4));
	Board[div8[index]] &= (~(15 << (mod8[index]*4)));
	Board[div8[index]] |= (value << (mod8[index]*4));
	//fprintf(stderr, "modify:%u\n", Board[div8[index]]);
}
// judge if the move is legal
bool CDCagent::IsLegal (unsigned  int* chess,int from_location_no,int to_location_no,int UserId ) {
	bool IsCurrent = true;
	int from_chess_no = locate(chess,from_location_no);
	int to_chess_no = locate(chess,to_location_no);
	int from_row = div4[from_location_no], to_row = div4[to_location_no];
	int from_col = mod4[from_location_no], to_col = mod4[to_location_no];
	if(mod8[from_chess_no] == 0 || to_chess_no == CHESS_COVER) {  
		IsCurrent = false;//can't move from empty/can't move to/from cover
	}
	else if (div8[from_chess_no] != UserId) {
		IsCurrent = false;//can't move oppenent's 
	}
	else if(to_chess_no != CHESS_EMPTY && (div8[from_chess_no] == div8[to_chess_no])) {
		IsCurrent = false;//can't eat friend
	}
	else if(to_chess_no == CHESS_EMPTY && abs(from_row-to_row) + abs(from_col-to_col)  == 1){ //legal move 
		IsCurrent = true;
	}	
	else if(mod8[from_chess_no] == 6) { //judge cannon eat
		int row_gap = from_row-to_row;
		int col_gap = from_col-to_col;
		int between_Count = 0;
		if(row_gap == 0 || col_gap == 0) {
			if(row_gap == 0)  {
				for(int i=1;i<abs(col_gap);i++) {
					int between_chess;
					if(col_gap>0) { between_chess = locate(chess,from_location_no-i);}
					else { between_chess = locate(chess,from_location_no+i) ; }
					if(between_chess != CHESS_EMPTY) { between_Count++; }
				}
			}
			else {
				for(int i=1;i<abs(row_gap);i++) {
					int between_chess;
					if(row_gap > 0) { between_chess = locate(chess,from_location_no-4*i);}
					else { between_chess = locate(chess,from_location_no+4*i);}
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
	else { // non cannon eat
		if( abs(from_row-to_row) + abs(from_col-to_col)  > 1) {
			IsCurrent = false;
		}
		else if(mod8[from_chess_no] == 7) {//pawn
			if(mod8[to_chess_no] != 1 && mod8[to_chess_no] != 7) {//pawn or king
				IsCurrent = false;
			}
		}
		else if(mod8[from_chess_no] == 1 && mod8[to_chess_no] == 7) {//king can't eat pawn
			IsCurrent = false;
		}
		else if(mod8[from_chess_no] > mod8[to_chess_no]) {
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
	sprintf(temp,"------------%s-------------",myColor);
	strcat(Mes,temp);
	for(int i = 3; i >= 0; i--){
		for(int j = 16; j >= 0; j-=16){
			int judge = (j == 16)? 1 : 0;
			sprintf(temp,"\n<%d> ",i*2 + judge+1);strcat(Mes,temp);
			for(int k = j; k < j+16; k+=4){
				sprintf(temp,"%5c",chess_name[(this->Board[i] >> k) & 15]);
				strcat(Mes,temp);
			}
		}
	}

	// strcat(Mes,"<8> ");
	// for(int i=0;i<32;i++){
	// 	if(i != 0 && i % 4 == 0){
	// 		sprintf(temp,"\n<%d> ",8-(i/4));
	// 		strcat(Mes,temp);
	// 	}
	// 	char chess_name[10]="";
	// 	Print_Chess(this->Board[i],chess_name);
	// 	sprintf(temp,"%5s", chess_name);
	// 	strcat(Mes,temp);
	// }
	strcat(Mes,"\n     ");
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

int CDCagent::firststep(){
	fprintf(stderr, "firststep\n" );
	int opponent_pos, opponent_kind,find = 0;
	for(int i = 0; i < 32 && !find; i++){
		int t = locate(this->Board, i);
		if(t != CHESS_COVER){
			find = 1;
			opponent_pos = i;
			opponent_kind = mod8[t];
		}
	}
	fprintf(stderr, "oppenent_pos:%d,opponent_kind:%d\n", opponent_pos,opponent_kind);
	int max = 0, maxvalue = -1;
	if(opponent_kind == 1 || opponent_kind == 6 || opponent_kind == 7){//K/C/P
		int canditates[4] = {opponent_pos+1,opponent_pos-1,opponent_pos+4,opponent_pos-4};
		for(int i = 0; i < 4; i++){
			int from_row = div4[opponent_pos], to_row = div4[canditates[i]];
			int from_col = mod4[opponent_pos], to_col = mod4[canditates[i]];
			int row_gap = from_row-to_row, col_gap = from_col-to_col;
			if(canditates[i] >= 0 && canditates[i] <32 && abs(row_gap)+abs(col_gap)==1){
				if(pos_value[canditates[i]] >maxvalue){
					maxvalue = pos_value[canditates[i]];
					max = canditates[i];
				}
			}
		}
	}
	else{
		int canditates[4] = {opponent_pos+2,opponent_pos-2,opponent_pos+8,opponent_pos-8};
		for(int i = 0; i < 4; i++){
			int from_row = div4[opponent_pos], to_row = div4[canditates[i]];
			int from_col = mod4[opponent_pos], to_col = mod4[canditates[i]];
			int row_gap = from_row-to_row, col_gap = from_col-to_col;
			if(canditates[i] > 0 && canditates[i] <32 && abs(row_gap)+abs(col_gap)==2){
				if(pos_value[canditates[i]] >maxvalue){
					maxvalue = pos_value[canditates[i]];
					max = canditates[i];
				}
			}
		}
	}
	fprintf(stderr, "Answer in firststep:%d,%d\n",max, (max<<7)+ max);
	return (max<<7)+max;
}

int CDCagent::SEE(unsigned int *Board){
	int best_move = -1, best_target = 0;
	for(int i = 0; i < 32; i++){
		int chess_no = locate(Board,i);
		int chess_color = div8[chess_no];
		int chess_kind = mod8[chess_no]-1;
		if(chess_no == CHESS_COVER || chess_no == CHESS_EMPTY || chess_color != this->Color) continue;

		int surround[4] = {i+1,i-1,i+4,i-4};
		for(int j = 0; j < 4; j++){
			if(surround[j] >= 0 && surround[j] < 32){
				int from_row = div4[i], to_row = div4[surround[j]];
				int from_col = mod4[i], to_col = mod4[surround[j]];
				int row_gap = from_row-to_row, col_gap = from_col-to_col;
				int dst_chess = locate(Board,surround[j]);
				int dst_kind = mod8[dst_chess]-1;
				if(dst_chess == CHESS_COVER || dst_chess == CHESS_EMPTY) continue;
				if((abs(row_gap)+abs(col_gap)==1)){
					if(canmove[chess_no*16+dst_chess] && basic_chess_value[dst_kind] > best_target){//capture
						best_move = (i << 7) + surround[j];
						best_target = basic_chess_value[dst_kind];
					}
					else if(canmove[dst_chess*16+chess_no] && basic_chess_value[chess_kind] > best_target){//threaten
						for(int k = 0; k < 4; k++){
							if(surround[k] >= 0 && surround[k] < 32){
								int dst_row = div4[surround[k]],dst_col = mod4[surround[k]];
								int run_chess = locate(Board,surround[k]);
								if((abs(from_row-dst_row)+abs(from_col-dst_col) == 1) && canmove[chess_no*16+run_chess]){
									best_move = (i << 7) + surround[k];
									best_target = basic_chess_value[chess_kind];
								}
							}
						}
					}
				}
			}
		}
	}
	return best_move;
}

void CDCagent::Play(char move[6]) {
	// move generation	
	int non_flip_moves[128];
	int total_non_flip = this->NonFlipList(this->Board, this->Color, non_flip_moves);
	int flip_moves[32];
	int total_flip = this->FlipList(this->Board,flip_moves);
	fprintf(stderr, "non_flip: %d, flip: %d\n", total_non_flip, total_flip);

	// move decision (searching happens here)
	int Answer = 129,depth_limit = 10;//time_to_depth((this->color)? this->Red_Time:this->Black_Time);
	
	if(this->plies == 0) Answer = ((21<<7)+21);
	else if(this->plies == 1) Answer = firststep();
	else{
		if(total_non_flip > 0){
			if(total_non_flip == 1){
				Answer = non_flip_moves[0];
			}
			else{
				int a= SEE(this->Board);
				if(a != -1) Answer = a;
				else 
					Answer = NegaMax(depth_limit);
			}
		}
		else if(total_flip>0){
			Answer = flip_moves[rand()%total_flip];
		}
		else {
			fprintf(stderr, "ERROR: no legal move\n");
			exit(1);
		}
	}
	// move translation 
	int startPoint = Answer>>7;
	int endPoint   = Answer & 127;
	fprintf(stderr, "Answer: %d %d\n", startPoint,endPoint);
	sprintf(move, "%c%c-%c%c",'a'+(mod4[startPoint]),'1'+div4[startPoint],'a'+mod4[endPoint],'1'+div4[endPoint]);
	
	char chess_Start[10] = "", chess_End[10] = "";
	Print_Chess(Board[startPoint],chess_Start);
	Print_Chess(Board[endPoint],chess_End);
	
	fprintf(stderr, "My move:\n");
	fprintf(stderr, "<%s> -> <%s>\n",chess_Start,chess_End);
	fprintf(stderr, "move:%s\n",move);
	this->Print_Chessboard();
}

int CDCagent::NegaMax(int depth_limit){
	unsigned int tmp_board[4] = {this->Board[0],this->Board[1],this->Board[2],this->Board[3]};
	unsigned int tmp_livechess = this->LiveChess;
	long long int tmp_hash_value = this->hash_value;
	int move = 0;
	F4(tmp_board,tmp_livechess,tmp_hash_value,-2147483640,2147483647,depth_limit, move);
	return move;
}


int max(int a, int b){
	return (a>b)?a:b;
}

int CDCagent::evaluate(unsigned int* Board,unsigned int LiveChess){
	//return rand() %1000 +1000;
	int chess_value_sum = 0;
	int dynamic_chess_value[14];
	int opponent_color = this->Color? RED:BLACK;
	for(int i = 0; i < 7; i++){// dynamic modify chess value
		//int chess_no = this->Color*8+i;
		dynamic_chess_value[i] = basic_chess_value[i];//TODO
		dynamic_chess_value[7+i] = basic_chess_value[i];
	}
	for(int i = 0; i < 7; i++){
		//int chess_no = this->Color*8+i;
		int livechessnum = (LiveChess>>(this->Color*14 + cumulate_chessdigit[i])) & ((1<<Chess_digit[i])-1);
		chess_value_sum += dynamic_chess_value[this->Color*7+i]*livechessnum;
		//chess_no = opponent_color*8+i;
		livechessnum = (LiveChess>>(opponent_color*14 + cumulate_chessdigit[i])) & ((1<<Chess_digit[i])-1);
		chess_value_sum -= dynamic_chess_value[opponent_color*7+i]*livechessnum;
	}
	// freedom bonus
	for(int i = 0; i < 32; i++){
		int surround[4] = {i+1,i-1,i+4,i-4};
		int chess_no = locate(Board,i);
		if(chess_no == CHESS_COVER || chess_no == CHESS_EMPTY) continue;

		int chess_color = div8[chess_no];
		int chess_kind = mod8[chess_no]-1;
		for(int j = 0; j < 4; j++){
			if(surround[j] >= 0 && surround[j] < 32){
				int from_row = div4[i], to_row = div4[surround[j]];
				int from_col = mod4[i], to_col = mod4[surround[j]];
				int row_gap = from_row-to_row, col_gap = from_col-to_col;
				int surround_chess = locate(Board,surround[j]);
				if((abs(row_gap)+abs(col_gap)==1) && canmove[chess_no*16+surround_chess])
					chess_value_sum += ((chess_color==this->Color)?div4_basic[chess_kind]:-div4_basic[chess_kind]);
			}
		}
	}
	//fprintf(stderr, "evaluate: %d\n", chess_value_sum);
	fflush(stderr);
	return chess_value_sum;
}

void CDCagent::move_order(unsigned int* Board, int* non_flip_moves, int total_non_flip){
	int best_value = 0,best_i = 0;
	for(int i = 0; i <total_non_flip; i++){
		int src = non_flip_moves[i]>>7;
		int chess_no = locate(Board,src);
		int chess_kind = mod8[chess_no]-1;
		if(basic_chess_value[chess_kind] > best_value){
			best_value = basic_chess_value[chess_kind];
			best_i = i;
		}
	}
	int tmp = non_flip_moves[0];
	non_flip_moves[0] = non_flip_moves[best_i];
	non_flip_moves[best_i] = tmp;
}

void CDCagent::do_move(unsigned int* Board, unsigned int& LiveChess,long long int &hash_value, int move, int flip) { 
	int src = move>>7;
	int dst = move&127;
	int src_chess = locate(Board,src);
	//int my_color = div8[src_chess];
	//int opponent_color = my_color? RED : BLACK;
	// hash_value ^= color_rand[opponent_color];
	// hash_value ^= color_rand[my_color];
	//fprintf(stderr, "%d\n", src);
	if(src == dst){ 
		//fprintf(stderr, "FLIP: %c%c = %c\n",move[0], move[1], move[3]);
		modify(Board,src,flip);
		hash_value ^= random_num[8*32+src];
		hash_value ^= random_num[flip*32+src];
		//Print_Chessboard();
	}else { 
		//fprintf(stderr, "MOVE: %c%c - %c%c\n",move[0], move[1], move[3], move[4]);
		int dst_chess = locate(Board,src); 
		hash_value ^= random_num[dst_chess*32+dst];
		hash_value ^= random_num[src_chess*32+dst];
		hash_value ^= random_num[src_chess*32+src];
		hash_value ^= random_num[CHESS_EMPTY*32+src];
		if(dst_chess != CHESS_EMPTY){
			int dst_color = div8[dst_chess];
			int dst_kind = mod8[dst_chess]-1;
			LiveChess -= (1 << (dst_color*14+cumulate_chessdigit[dst_kind]));//low red high black
		}
		modify(Board,dst,src_chess);
		modify(Board,src,CHESS_EMPTY);
		//Print_Chessboard();
	}
}

int CDCagent::F4(unsigned int* Board,unsigned int LiveChess,long long int hash_value, int alpha, int beta, int depth, int& answer){
	//fprintf(stderr,"depth = %d\n",depth);
	if(depth == 0){
		return evaluate(Board, LiveChess);
	}

	int opponent_color = (this->Color)? RED: BLACK;
	int Color = (depth &1)? opponent_color:this->Color;
	int m = -2147483647, n = beta;
	int best_move,answer_next;
	int hash_index = hash_value & ((1 << HASH_TABLE_SIZE_N) - 1);
	table_entry* cur = &trans_table[Color][hash_index];
	if(cur->exact > 0){
		if(cur->hash_value == hash_value){
			//fprintf(stderr, "depth:%d hash hit!\n",depth);
			if(cur->depth < depth){
				if (cur->exact == 1){
					best_move = cur->best_move;
					m = cur->best_value;
				}
			}
			else{
				if(cur->exact == 1){
					answer = cur->best_move;
					return cur->best_value;
				}
				if(cur->exact == 2){
					if(cur->best_value > m){
						m = cur->best_value;
						best_move = cur->best_move;
					}
					if(m >= beta){
						answer = best_move;
						return m;
					}
				}
				else{
					if(cur->best_value < n){
						n = cur->best_value;
						best_move = cur->best_move;
					}
					if(n <= alpha){
						answer = best_move;
						return n;
					}
				}
			}
		}
		// else
		// 	fprintf(stderr, "depth:%d collision!\n",depth);
	}

	// move generation	
	int non_flip_moves[128];
	int total_non_flip = NonFlipList(Board, Color, non_flip_moves);
	int flip_moves[32];
	int total_flip = FlipList(Board, flip_moves);
	//fprintf(stderr, "non_flip: %d, flip: %d\n", total_non_flip, total_flip);
	if(total_non_flip > 0){
		move_order(Board,non_flip_moves,total_non_flip);
	}
	for(int i = 0; i < total_non_flip; i++){
		int move = non_flip_moves[i];
		//int src = move >>7, dst = move & 127;
		unsigned int tmp_board[4] = {Board[0],Board[1],Board[2],Board[3]},tmp_livechess = LiveChess;
		long long int tmp_hash_value = hash_value;
		do_move(tmp_board,tmp_livechess,tmp_hash_value,move,0);
		int t = -F4(tmp_board,tmp_livechess,tmp_hash_value,-n,-max(alpha,m),depth-1,answer_next);
		//fprintf(stderr, "depth %d src:%d dst: %d ",depth, src,dst);
		//fprintf(stderr, "has scout value: %d\n",t);
		if(t > m){
			best_move = move;
			if(n == beta || depth < 3 || t >= beta){
				m = t;
			}
			else
				m = -F4(tmp_board,tmp_livechess,tmp_hash_value,-beta,-t,depth-1,answer_next);
		}
		if(m >= beta){//beta cutoff
			answer = move;
			cur->hash_value = hash_value;
			cur->depth = depth;
			cur->best_value = m;
			cur->best_move = answer;
			cur->exact = 2;
			return m;
		}
		n = max(alpha,m) + 1;
	}
	//chance_node
	unsigned int CoverChess = LiveChess;
	int CoverChessnum[14];
	if(total_flip < 3){
		for(int i =0; i < 32; i++){
			int chess_no = locate(Board,i);
			int chess_color = div8[chess_no];
			int chess_kind = mod8[chess_no]-1;
			if(chess_no != CHESS_COVER || chess_no != CHESS_EMPTY){
				CoverChess -= (1 << (chess_color*14 + cumulate_chessdigit[chess_kind]));
			}
		}
		for(int i = 0; i < 7; i++){
			CoverChessnum[i] = (CoverChess>>(cumulate_chessdigit[i])) & ((1<<Chess_digit[i])-1);
			CoverChessnum[7+i] = (CoverChess>>(14+cumulate_chessdigit[i])) & ((1<<Chess_digit[i])-1);
		}
		for(int i = 0; i < total_flip; i++){
			int move = flip_moves[i];
			unsigned int tmp_board[4] = {Board[0],Board[1],Board[2],Board[3]},tmp_livechess = LiveChess;
			long long int tmp_hash_value = hash_value;
			int exp = 0;
			for(int j = 0; j < 14; j++){
				if(CoverChessnum[j] > 0){
					int chess_no = (j < 7)? j+1 : j+2;
					do_move(tmp_board, tmp_livechess,tmp_hash_value ,move, chess_no);
					int tmp = evaluate(tmp_board,tmp_livechess);
					int partial = (depth & 1)? -tmp:tmp;
					exp += partial/total_flip*CoverChessnum[j];
				}
			}//
			if(exp > m){
				m = exp;
				best_move = move;
			}
			if(m >= beta){//beta cutoff
				answer = move;
				cur->hash_value = hash_value;
				cur->depth = depth;
				cur->best_value = m;
				cur->best_move = answer;
				cur->exact = 2;
				return m;
			}
		}
	}
	if(m == -2147483647){
		int tmp = evaluate(Board,LiveChess);
		m = (depth & 1)? -tmp:tmp;
	} 
	//fprintf(stderr, "depth %d best_move:%d, max:%d\n",depth,best_move,m);
	answer = best_move;
	cur->hash_value = hash_value;
	cur->depth = depth;
	cur->best_value = m;
	cur->best_move = answer;
	cur->exact = (m > alpha)?1:3;
	return m;
}