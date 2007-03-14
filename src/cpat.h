/*****************************************************************************
 *                                                                           *
 * cpat.h - header for cpat.c                                                *
 *                                                                           *
 *            Copyright (C) 2006 Trevor Carey-Smith                          *  
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU General Public License version 2 as published  *
 * by the Free Software Foundation. (See the COPYING file for more info.)    *
 *                                                                           *
 *****************************************************************************/

#ifndef CPAT_CPAT_H
#define CPAT_CPAT_H 1

#if HAVE_CONFIG_H
#   include <config.h>
#endif

#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /*STDC_HEADERS*/

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include <unistd.h>
extern char *optarg;
extern int  optind;
#endif

/*
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
*/

#include <signal.h>
#include <time.h>

#if HAVE_NCURSES_H
#  include <ncurses.h>
#elif HAVE_CURSES_H
#  include <curses.h>
#endif

#if !STDC_HEADERS
#  if !HAVE_STRCHR
#    define strchr index
#    define strrchr rindex
#  endif
#endif

/* If system doesn't have nanosleep, we replace with usleep */
#if !HAVE_NANOSLEEP
#  if !HAVE_STRUCT_TIMESPEC
struct timespec { 
    long    tv_nsec; 
};
#  endif
#  if HAVE_USLEEP
#    define nanosleep(req,rem)    ((void) usleep (*req.tv_nsec/1000))
#  else
#    define nanosleep(req,rem)	
#  endif
#endif /* !HAVE_NANOSLEEP */

#if !HAVE_TIME_T
typedef long time_t;
#endif

/* Time to pause when doing auto moves (millisec).
 * This number is passed to nanosleep(). */
#define PAUSETIME	400

/* Define new games here. Need to add its full name to names below */
#define FCELL		0
#define KLOND		1
#define SPIDER		2
#define FORTYTH		3
#define BAKERSD		4
#define SCORP		5
#define PENGUIN		6
#define YUKON		7
#define STRATEGY	8
#define NUM_GAMES	9

/* This produces white space */
#define NOCARD		(-1)
/* This produces a blank black card */
#define CARDSPACE	(-2)
/* These are for collapsed sequences */
#define CARDSEQR	(-3)
#define CARDSEQB	(-4)
/* This is for a deck of cards. The number of cards in the pile 
 * written on a blue card. Cards less than this number will be blue. */
#define CARDBACK	(-5)

#define SUIT_LENGTH	13
#define NUM_SUITS	4
#define KING		12
#define ACE		0

/* These are for checking card sequences */
#define ASC		0
#define DESC		1
#define ANY_ORDER	2
#define ALT_COL		0
#define IN_SUIT		1
#define ANY_SUIT	2
#define WRAP		0
#define NO_WRAP		1
#define FACE_UP		0
#define FACE_DOWN	100

#define BACK_COLOR	4
#define HEARTS_COLOR	6
#define SPADES_COLOR	7

#define PACK_SIZE	52

#define MAX_NUM_COLS	13
#define MAX_COL_LEN	66   
/* It is not possible to have more than 19 cards in a column */
/* unless playing spider, when technically you could have 66 ! */

#define CARD_WIDTH	5
#define CARD_HEIGHT	2
/* positioning info */
/* How far down the main board is. */
#define BOARD_Y		1
/* How far down the side boards are. */
#define PILE_Y		2
/* How far across the first side board is. */
#define PILE_X		1
/* The size of the board borders */
#define BORDER		1
/* The size of the space between the boards */
#define SPACE		1

/* These are for the undo type */
#define UNDO_NORMAL	0
#define UNDO_FACE_DOWN	1
#define UNDO_DISCARD	2
#define UNDO_ROLLCARDS	3
#define UNDO_DEAL	4
#define UNDO_IMPOSSIBLE	5

typedef struct undo_node
{
    int number,src,dst;
    int type;
    struct undo_node * nextnode;
} undo_node;

typedef struct {
    WINDOW *main,*free,*found,*input,*hint;
    int hint_h;
    int game;
    /* The columns on the main board */
    int cols[MAX_NUM_COLS][MAX_COL_LEN]; 
    /* Keeps track of how many cards in each column (0 means 1 card) */
    int col_size[MAX_NUM_COLS];
    undo_node * undo;
    int deck[2*PACK_SIZE];
    int face_down,face_up; /* Number of cards in deck and discard (KLON,SPID)*/
    int turn_no; /* this is the number of cards to turn over (Klondike) */
    int freepile[8]; /* The cards to show on the free board */
    int foundation[8]; /* The cards to show on the foundations */
    int foun_size[8]; /* Number of cards in each foundation */
    int foun_dir;
    int num_cols;
    int num_foun;
    int num_free;
    int num_packs;
    int num_deals; /* number of times allowed to deal through deck */
    int foun_start;
    int allow_undo;
    int difficulty;

    /* To notify draw_piles() which columns to draw */
    int print_col[MAX_NUM_COLS]; 

    /* Game stats */
    int seed,moves,deals,finished_foundations;

    int debug;
} GameInfo;
    
struct timespec pauselength;
struct timespec pauseleft;

/* This is the background color of the boards */
static chtype boardbkgd = ' ' | COLOR_PAIR(SPADES_COLOR);

/* Game names */
static char *names[NUM_GAMES] =
    {"FreeCell","Klondike","Spider","40 Thieves","Baker's Dozen","Scorpion",
    "Penguin","Yukon","Strategy"};

undo_node *pop_items(undo_node *stackTop);
undo_node *push_items(undo_node *stackTop,int src,int dst,int number,int type);
void clear_undo(GameInfo* g);
int menu(int num_items,char **items,char *title,char *query,int num_phrases, char **phrases);

void freecell(GameInfo* g);
void klondike(GameInfo* g);
void spider(GameInfo* g);
void fortythieves(GameInfo* g);
void bakersdozen(GameInfo* g);
void scorpion(GameInfo* g);
void penguin(GameInfo* g);
void yukon(GameInfo* g);
void strategy(GameInfo* g);

void die(int onsig);

#endif /* !CPAT_CPAT_H */
/* cpat.h ends here */
