/*****************************************************************************
 *                                                                           *
 * common.h - header for common.c                                            *
 *                                                                           *
 *            Copyright (C) 2006 Trevor Carey-Smith                          *  
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU General Public License version 2 as published  *
 * by the Free Software Foundation. (See the COPYING file for more info.)    *
 *                                                                           *
 *****************************************************************************/

#ifndef CPAT_COMMON_H
#define CPAT_COMMON_H 1

//static char *ranks[SUIT_LENGTH] =
//        {" A"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9"," T"," J"," Q"," K"};
static char ranks[] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
#if HAVE_NCURSESW
// 4 suits columns are for (1) main suit character, (2) character to
// use as a prefix to the rank, (3)+(4) as for (1) and (2) but for when
// ascii is forced
static char *suits[4][NUM_SUITS] =
    {   
        {"\xe2\x99\xa5","\xe2\x99\xa0","\xe2\x99\xa6","\xe2\x99\xa3"},
        {"\xe2\x99\xa1","\xe2\x99\xa4","\xe2\x99\xa2","\xe2\x99\xa7"},
        {"H","S","D","C"},
        {" "," "," "," "}
    };
// second version is for when ascii is forced
static char *carddesign1[2] = {"\xe2\x98\xbc","+"};
static char *carddesign2[2] = {"\xe2\x98\xbe\xe2\x98\xbd","++"};
static char *carddesign3[2] = {"\xe2\x98\xbe\xe2\x98\xbc\xe2\x98\xbd"," + "};
// static char *carddesign = "\xe2\x99\x9b";
#else
static char *suits[4][NUM_SUITS] =
    {   
        {"H","S","D","C"},
        {" "," "," "," "},
        {"H","S","D","C"},
        {" "," "," "," "} 
    };
static char *carddesign1[2] = {"+","+"};
static char *carddesign2[2] = {"++","++"};
static char *carddesign3[2] = {" + "," + "};
#endif

int create_windows(GameInfo* g);
void kill_windows(GameInfo* g);
int game_finished(GameInfo* g,char* game_str);
void init_deck(GameInfo* g);
void deal_deck(int type, GameInfo* g);
void turnover_waste(GameInfo* g);
void foundation_automove(int number,GameInfo* g);
void roll_deckcards(GameInfo* g);
int check_sequence(int number,int col,int direction,int type,int wrap,GameInfo* g);
int check_move(int col,int card,int direction,int type,int wrap,GameInfo* g);
int find_move(int src,int dst,int direction,int type,int wrap,GameInfo* g);
int grab_input(GameInfo* g, int* src, int* dst, int* number);
void printcard(WINDOW *win,int y,int x,int value,GameInfo* g);
void draw_piles(WINDOW *win, GameInfo* g);
void init_hints(WINDOW *win,GameInfo* g);
void init_board(WINDOW *win,GameInfo* g);
void draw_screen(GameInfo* g);
void move_card(int src,int dst,int number,GameInfo* g);
void show_error(char* error_str,WINDOW* win);
void dump_vars(GameInfo* g);


#endif /* !CPAT_COMMON_H */
/* common.h ends here */
