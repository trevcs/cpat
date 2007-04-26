/*****************************************************************************
 *                                                                           *
 *                              CPat                                         *
 *                         =================                                 *
 *            A curses based implementation of solitaire                     *
 *            Copyright (C) 2006 Trevor Carey-Smith                          *  
 *                                                                           *
 *          The program outline and some of the basic functions are          *
 *          taken from bluemoon by T. A. Lister and Eric S. Raymond.         *
 *                     http://www.catb.org/~esr/bluemoon/                    *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU General Public License version 2 as published  *
 * by the Free Software Foundation.                                          *
 *                                                                           *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY*
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License   *
 * for more details.                                                         *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with this program (the COPYING file); if not, write to the Free Software  *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                           *
 *****************************************************************************/

#include "cpat.h"
#include "highscores.h"

/* To catch kill signals and exit cleanly */
void
die(int onsig)
{
    if (hs.available) write_hs();
    signal(onsig, SIG_IGN);
    endwin();
    exit(0);
}

/* Code to provide a stack (linked list) to store moves so they can be 
 * undone...
 */
undo_node *push_items(undo_node *stackTop,int src,int dst,int number,int type)
{
    static undo_node *newItem;
	
    newItem = calloc(sizeof(undo_node),1);
    newItem->src = src;
    newItem->dst = dst;
    newItem->number = number;
    newItem->type = type;
    
    newItem->nextnode = stackTop;
	
    return newItem;
}

undo_node *pop_items(undo_node *stackTop)
{
    static undo_node *temp;
   
    temp = stackTop->nextnode;
    free(stackTop);
	
    return temp;
}

void clear_undo(GameInfo* g)
{
    /* Clean up all the undo levels */
    while (g->undo != NULL) g->undo = pop_items(g->undo);
}

/* Creates windows to write stuff not in a game */
int
pager(char *title,char* text,int num_phrases, char **phrases)
{
    WINDOW *main,*input;
    char inp;
    int title_y;
    int phrases_y,i;
    int inner_w,inner_h,inner_x,inner_y;
    int outer_w,outer_h,outer_x,outer_y;

    int y,maxy,maxx;
    int page_width;
    char *start_p,*end_p,*final_p;
    char dest[200]; /* this will contain each line before we print it */

    /* Initial values for outer window */
    outer_w = COLS-8;
    outer_h = LINES-4;
    outer_x = 4;
    outer_y = 2;

    inner_w = outer_w-8;
    inner_x = outer_x+(outer_w-inner_w)/2;
    inner_h = outer_h-5-num_phrases;
    inner_y=outer_y+4+num_phrases;
    title_y = 2;
    phrases_y = 4;

    main = newwin(outer_h,outer_w,outer_y,outer_x);

    wbkgdset(main, boardbkgd);
    wclear(main);
    box(main, 0, 0);

    wattron(main,A_UNDERLINE);
    mvwprintw(main,title_y,4,title);
    wattroff(main,A_UNDERLINE);
    for (i=0;i<num_phrases;i++) 
	mvwprintw(main,phrases_y+i,4,phrases[i]);
    wrefresh(main);

    input = newwin(inner_h,inner_w,inner_y,inner_x);
    wbkgdset(input, boardbkgd);
//    wattron(input,A_REVERSE | COLOR_PAIR(BACK_COLOR) | A_BOLD);
//    wbkgdset(input, COLOR_PAIR(BACK_COLOR) | A_REVERSE | A_BOLD);
    wclear(input);

    wmove(input,0,0);

    getmaxyx(input,maxy,maxx);
    page_width = maxx-1;

    /* points to last character of text */
    final_p=&text[0]+strlen(text);

    /* will point to last character of current line */
    end_p = &text[0];
    /* will point to first character of current line */
    start_p = &text[0];
    /* do this for all of text */
    while (end_p < final_p)
    {
	end_p = start_p;

	/* search from start of current line for newline chr */
	while (*end_p != '\n' && end_p <= start_p+page_width) end_p++;

	/* If no newline found */
	if (end_p == start_p + page_width + 1)
	{
	    end_p--;
    	    if (start_p + page_width >= final_p)
    		end_p=final_p;
	    else
		/* search for the space closest to the end of current line*/
		while (*end_p != ' ' && end_p>start_p) end_p--;
	}
	
	if (end_p==start_p && *end_p != '\n')
	{
	    /* This means there's no space on the line, we will
	     * break the long line up */
	    end_p = start_p + page_width;
	    (void) strncpy(dest,start_p,(int)(end_p-start_p));
	    dest[(int)(end_p-start_p)]='\0';
	    start_p=end_p;
	}
	else
	{
	    (void) strncpy(dest,start_p,(int)(end_p-start_p));
    	    dest[(int)(end_p-start_p)]='\0';
	    start_p=end_p+1;
	}
	wprintw(input,"%s\n",dest);

	getyx(input,y,maxx);
	/* check if we are at the bottom of the window
	 * or if this is the last line of the last paragraph */
	if (y>maxy-3 || (end_p>=final_p))
	{
	    getmaxyx(input,maxy,maxx);
    	    mvwprintw(input,maxy-1,maxx-16,"press a key...");
//	    box(input,0,0);
	    wrefresh(input);
	    wgetch(input);
	    wclear(input);
	    wmove(input,0,0);
	}
    }

    /* Now delete windows */
    delwin(input);
    delwin(main);
    clear();
    refresh();
}

/* Creates a blue menu type screen */
int
menu(char *title,
	int num_items,char **items,char *query,
	int num_items2,char **items2,char *query2,
	int num_phrases, char **phrases)
{
    WINDOW *main,*input;
    char inp;
    int title_y;
    int phrases_y;
    int inner_w,inner_h,inner_x,inner_y;
    int outer_w,outer_h,outer_x,outer_y;
    int lines,lines2,i;
    int name_len=0;


    /* Initial values for outer window */
    outer_w = COLS-8;
    outer_h = LINES-4;
    outer_x = 4;
    outer_y = 2;
    title_y = 2;
    phrases_y = 4;
    
    for (i=0;i<num_items;i++)
	if (strlen(items[i])>name_len)
	    name_len=strlen(items[i]);
    for (i=0;i<num_items2;i++)
	if (strlen(items2[i])>name_len)
	    name_len=strlen(items2[i]);

    lines=num_items;
    lines2=num_items2;

    /* First make adjustments to width of windows */
    inner_w = 2*(name_len+10);
    if ((num_items<5 && num_items2<3) || inner_w>outer_w)
    {
	/* Outer win way too thin, change to one column view */
//	inner_w=COLS-14;
	inner_w = (name_len+14);
	if (inner_w < 37) inner_w=37;
	if (inner_w > COLS-14) inner_w=COLS-14;
    }
    else if (inner_w>outer_w-4)
    {
	/* Outer win only just to thin, increase it by 4 */
	outer_w=COLS-4;
	outer_x=2;
	lines=(lines+1)/2;
	lines2=(lines2+1)/2;
    }
    else
    {
	lines=(lines+1)/2;
	lines2=(lines2+1)/2;
    }

    if (num_items2==0) lines2=-1; // removes extra space when no second list

    inner_x = outer_x+(outer_w-inner_w)/2;

    inner_h = lines+5+lines2+1;
    inner_y=outer_y+5+num_phrases;;
    if (inner_h>outer_h-2-num_phrases)
    {
	title_y -= 1;
	phrases_y -= 2;
	outer_h += 2;
	outer_y -= 1;
	inner_y -= 4;
    }
    else if (inner_h>outer_h-3-num_phrases)
    {
	title_y -= 1;
	phrases_y -= 1;
	outer_h += 2;
	outer_y -= 1;
	inner_y -= 3;
    }
    else if (inner_h>outer_h-4-num_phrases)
    {
	outer_h += 2;
	outer_y -= 1;
	inner_y -= 2;
    }
    else if (inner_h>outer_h-7-num_phrases)
    {
	outer_h += 2;
	outer_y -= 1;
	inner_y -= (num_phrases)?1:2;
    }
    else if (inner_h>outer_h-9 && num_phrases==0)
	inner_y -= 1;

    if (inner_h > LINES-inner_y)
    {
	endwin();
	puts ("CPat Error: your screen is too small.");
	exit (1);
    }

    main = newwin(outer_h,outer_w,outer_y,outer_x);

    wbkgdset(main, boardbkgd);
    wclear(main);
    box(main, 0, 0);

    wattron(main,A_UNDERLINE);
    mvwprintw(main,title_y,4,title);
    wattroff(main,A_UNDERLINE);
    for (i=0;i<num_phrases;i++) 
	mvwprintw(main,phrases_y+i,6,phrases[i]);
    wrefresh(main);

    input = newwin(inner_h,inner_w,inner_y,inner_x);
    wattron(input,A_REVERSE | COLOR_PAIR(BACK_COLOR) | A_BOLD);
    wbkgdset(input, COLOR_PAIR(BACK_COLOR) | A_REVERSE | A_BOLD);
    wclear(input);

    mvwaddstr(input, 1,2,query);
    for (i=0;i<num_items;i++)
	mvwprintw(input,2+i%lines,i<lines?4:name_len+12,"[%c] - %s",'a'+i,items[i]);
    mvwaddstr(input, 1+lines+1,2,query2);
    for (i=0;i<num_items2;i++)
	mvwprintw(input,2+lines+1+i%lines2,i<lines2?4:name_len+12,"[%c] - %s",'a'+i+num_items,items2[i]);

    lines = (NUM_GAMES < lines) ? NUM_GAMES : lines;
    do {
	mvwaddstr(input,lines+3+lines2+1,2,"Enter your choice ('q' to quit): ");
    } while (!(inp = wgetch(input)) || 
	    (inp!='q' && (inp<'a'||inp>='a'+num_items+num_items2)));

    if (inp=='q')
	die(0);

    delwin(input);
    delwin(main);
    clear();
    refresh();
    return (inp-'a');
}

main(int argc, char **argv, char *envp[])
{
#ifdef HAVE_GETOPT_LONG
    struct option  long_options [] = {
       	{ "seed", required_argument, 0, 's' },
       	{ "no-record", no_argument, 0, 'R' },
	{ "help", no_argument, 0, 'h' },
	{ "fast", no_argument, 0, 'f' },
	{ "cheat", no_argument, 0, 'c' },
	{ "debug", no_argument, 0, 'd' },
	{ "version", no_argument, 0, 'V' },
	{ NULL, 0, NULL, 0}
    };
#endif
    char title[40];
    char *short_options="s:RdfchV";
    char *home;
    int  help_flag = 0;
    int  version_flag = 0;
    int  error_flag = 0;
    int  fast_flag = 1;

    hs.available=TRUE;

    GameInfo g;
    g.debug = 0;
    g.allow_undo = 0;
    g.undo = NULL;
    const char *prog_name;
    
    g.seed = (int)time((time_t *)0);

    /* `basename' seems to be non-standard.  So we avoid it.  */
    prog_name = strrchr (argv[0], '/');
    prog_name = strdup (prog_name ? prog_name+1 : argv[0]);


    (void) signal(SIGINT, die);

    while (! error_flag) {
	int  c;
#ifdef HAVE_GETOPT_LONG
	int  ind;
	c = getopt_long (argc, argv, short_options, long_options, &ind);
#else
       	c = getopt (argc, argv, short_options);
#endif
	if (c == -1)  break;
	switch (c) {
	    case 's':
		g.seed = atoi(optarg);
		break;
	    case 'R':
		hs.available = FALSE;
		break;
	    case 'd':
		g.debug = 1;
		break;
	    case 'c':
		g.allow_undo = 1;
		break;
	    case 'h':
		help_flag = 1;
		break;
	    case 'f':
		fast_flag*=2;
		break;
	    case 'V':
		version_flag = 1;
		break;
	    default:
		error_flag = 1;
	}
    }
    if (argc != optind) {
	fputs ("too many arguments\n", stderr);
	error_flag = 1;
    }
    if (version_flag) {
	puts (PACKAGE_STRING);
	if (! error_flag)  exit (0);
    }
    if (error_flag || help_flag) {
#ifdef HAVE_GETOPT_LONG
#  define P(both,short,long) "  " both ", " long "    "
#else
#  define P(both,short,long) "  " both " " short "    "
#endif
    	FILE *out = error_flag ? stderr : stdout;
	fprintf (out, "usage: %s [OPTION...]\n", prog_name);
	fprintf (out, 
		"%s - a curses based solitaire collection\n\n", PACKAGE_NAME);
	fputs (P("-h","    ","--help     ") "print this message\n", out);
	fputs (P("-f","    ","--fast     ") "for faster auto-moves\n", out);
	fputs (P("-s","SEED","--seed=SEED") "seed for the deck shuffle\n", out);
	fputs (P("-R","    ","--no-record") "do not record game stats\n", out);
	fputs (P("-c","    ","--cheat    ") "allow undo for all moves\n", out);
	fputs (P("-V","    ","--version  ") "print the version number\n", out);
	fputs ("\nPlease report bugs to " PACKAGE_BUGREPORT ".\n", out);
	exit (error_flag);
#undef P
    }

    home = getenv("HOME");
    if (home == NULL) 
    {
#if HAVE_PWD_H
	home = getpwuid(getuid())->pw_dir; 
#else
	hs.available=FALSE;
#endif
    }
    if (hs.available) 
    {
	(void)strncpy(hs.filename,home,100);
	(void)strncat(hs.filename,"/.cpat_scores",13);

	initialise_hs();
	read_hs();
    }

    pauselength.tv_nsec= PAUSETIME*1000000/fast_flag;

    srand(g.seed);

    initscr();
    start_color();
    init_pair(	HEARTS_COLOR,	COLOR_WHITE,	COLOR_RED);
    init_pair(	SPADES_COLOR,	COLOR_BLACK,	COLOR_WHITE);
    init_pair(	BACK_COLOR,	COLOR_BLUE,	COLOR_WHITE);

    cbreak();
    nonl();
    noecho();
    intrflush(stdscr,FALSE);
    keypad(stdscr,TRUE);

    char *items[5] = {"Help","Credits","License","Game Stats","High Scores"};

    while (1)
    {
	/* g.num_deals=0 so that hs ignores numdeals unless this is set */
	g.num_deals = 0;
	g.variation = 0;
	snprintf(title,40,"Welcome to CPat %s",VERSION);
	g.game=menu(title,NUM_GAMES,names,"Choose a game:",hs.available?5:3,items,"or an option:",0,names);
	switch (g.game) {
	    case FCELL:
		freecell(&g);
		break;
	    case KLOND:
		klondike(&g);
		break;
    	    case SPIDER:
    		spider(&g);
		break;
    	    case FORTYTH:
    		fortythieves(&g);
		break;
    	    case BAKERSD:
    		bakersdozen(&g);
		break;
    	    case SCORP:
    		scorpion(&g);
		break;
    	    case PENGUIN:
    		penguin(&g);
		break;
    	    case YUKON:
    		yukon(&g);
		break;
    	    case STRATEGY:
    		strategy(&g);
		break;
	    case NUM_GAMES:
		help();
		break;
	    case NUM_GAMES+1:
		credits();
		break;
	    case NUM_GAMES+2:
		license();
		break;
	    case NUM_GAMES+3:
		game_stats(0);
		break;
	    case NUM_GAMES+4:
		game_stats(1);
		break;
	}
	srand(++g.seed);
    }

    clear_undo(&g);
    die(0);
}
/* cpat.c ends here */
