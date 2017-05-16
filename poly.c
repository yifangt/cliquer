/* poly.c (needs tetromino.h)
This program demonstrates packing
tetrominoes (pieces made of four squares of equal size arranged 
with coincident sides) on an n x m board so that no more tetrominoes
can fit.  It takes four command-line arguments: 
poly n m min max
where n and m are the dimensions of the board and min and max are 
passed to clique_find_all as the number of pieces to fit on the 
board.  For example,
poly 5 5 3 3 
prints all the ways to fit three tetrominoes on a 5 x 5 board so that 
no other tetrominoes could fit, and
poly 6 5 0 0 
finds all the ways to fit as many pieces as possible on a 6 x 5 board. 
This program was inspired by Solomon W. Golomb's Puzzle Column 
"Placing Pentominoes on Boards" in the IEEE Information Theory Society 
Newsletter, Vol. 52, No. 2, June 2002, p. 7. */

#include <stdio.h>
#include <stdlib.h>
#include "cliquer.h"

#define POLYSIZE 4

struct polynomino {
	char *name;
	int sizex, sizey;
	char block[POLYSIZE*POLYSIZE+1];
};

#include "tetromino.h"

#define VALUE(p,x,y) ((p)*boardsize+(y)*boardx+(x))
#define PIECE(p) ((p)/(boardsize))
#define POSX(p)  ((p)%boardx)
#define POSY(p)  (((p)%(boardsize))/boardx)

static int boardx,boardy;
static int boardsize;
static int minpieces;
static int maxpieces;

static graph_t *generate_graph(void);
static boolean print_clique(set_t s, graph_t *g, clique_options *opts);

int main(int argc, char *argv[]) {
	graph_t *g;

	if (argc!=5) {
		fprintf(stderr,"%s <size_x> <size_y> <min_pieces> "
			"<max_pieces>\n",argv[0]);
		return 1;
	}
	boardx=atoi(argv[1]);
	boardy=atoi(argv[2]);
	boardsize=boardx*boardy;
	minpieces=atoi(argv[3]);
	maxpieces=atoi(argv[4]);
	if ((boardx <= 0) || (boardy <= 0) || (maxpieces<0) || (minpieces<0)) {
		fprintf(stderr,"Illegal arguments.\n");
		return 1;
	}

	printf("Gen graph...\n");
	g=generate_graph();
	printf("Testing...\n");
	graph_test(g,stdout);

	clique_default_options->output=stderr;
	clique_default_options->user_function=print_clique;
	clique_find_all(g,minpieces,maxpieces,TRUE,NULL);

	return 0;
}


static boolean collides(int t1, int x1, int y1, int t2, int x2, int y2) {
	int x,y;

	for (x=MAX(x1,x2); x<MIN(x1+tetromino[t1].sizex,
				 x2+tetromino[t2].sizex); x++)
		for (y=MAX(y1,y2); y<MIN(y1+tetromino[t1].sizey,
					 y2+tetromino[t2].sizey); y++)
			if (tetromino[t1].block[(y-y1)*POLYSIZE+(x-x1)]=='#' &&
			    tetromino[t2].block[(y-y2)*POLYSIZE+(x-x2)]=='#')
				return TRUE;
	return FALSE;
}

static graph_t *generate_graph(void) {
	graph_t *g;
	int t1,t2,x1,x2,y1,y2;

	g=graph_new(TETROMINOS*boardsize);

	for (t1=0; t1<TETROMINOS; t1++) {
		for (x1=0; x1<boardx; x1++) {
			if (x1+tetromino[t1].sizex > boardx)
				break;
			for (y1=0; y1<boardy; y1++) {
				if (y1+tetromino[t1].sizey > boardy)
					break;
		for (t2=0; t2<TETROMINOS; t2++) {
			for (x2=0; x2<boardx; x2++) {
				if (x2+tetromino[t2].sizex > boardx)
					break;
				for (y2=0; y2<boardy; y2++) {
					if (y2+tetromino[t2].sizey > boardy)
						break;
					if (!collides(t1,x1,y1,t2,x2,y2))
						GRAPH_ADD_EDGE(g,
							       VALUE(t1,x1,y1),
							       VALUE(t2,x2,y2));
				}
			}
		}
			}
		}
	}
	return g;
}



static boolean print_clique(set_t s, graph_t *g, clique_options *opts) {
	static char *buf=NULL;
	int i,j,c,n;
	int t,x,y;

	if (buf==NULL) {
		buf=malloc((boardx*2+1)*boardy+10);
	}

	c=0;
	for (i=0; i<boardy; i++) {
		for (j=0; j<boardx; j++) {
			buf[c++]='.';
			buf[c++]=' ';
		}
		buf[c++]='\n';
	}
	buf[c]=0;

	n=-1;
	c='A';
	while ((n=set_return_next(s,n)) >= 0) {
		t=PIECE(n);
		x=POSX(n);
		y=POSY(n);

		if (x+tetromino[t].sizex > boardx)
			return TRUE;
		if (y+tetromino[t].sizey > boardy)
			return TRUE;

		for (i=0; i<POLYSIZE; i++) {
			for (j=0; j<POLYSIZE; j++) {
				if (tetromino[t].block[i+j*POLYSIZE]=='#') {
					buf[(j+y)*(boardx*2+1)+(i+x)*2]=c;
				}
			}
		}
		c++;
	}

	printf("%d pieces:\n%s\n",set_size(s),buf);
	return TRUE;
}

