#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>

#define BRICKSIZE 4
#define BOARDWIDTH 10 
#define BOARDHEIGHT 22
#define MOVE_DOWN 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3

int score_define[]={0,5,12,21,32};

char brick_code[][BRICKSIZE*BRICKSIZE]={"0000000001100110","0000000011000110","1000100010001000","0000000010001110","0000000000101110","0000000001001110","0000000001101100"};


struct Board
{
	char blocks[BOARDHEIGHT+3][BOARDWIDTH+6];
};
struct Brick
{
	char blocks[BRICKSIZE][BRICKSIZE];
	int x;
	int y;
};

typedef struct Brick Brick;
typedef struct Board Board;

Brick *cur_brick;
Brick *next_brick;
Board board;
pthread_t thread1;
pthread_t thread2;
int score=0;
int count=0;
int speed;

void init_board();
void init_brick();
void new_brick();
void create_brick(Brick *brick);
int move_brick(int direction);
void change(Brick *brick, Board *board);
int delete();
void print_brick(char blocks[BRICKSIZE][BRICKSIZE]);
void print_board();
void operate();
void time_pass();
int delete();

int main()
{
	char yn;

	initscr();
	keypad(stdscr,1);
	timeout(500);
	noecho();

	srand(time(NULL));
	init_board();
	init_brick();
	
	print_board();
		


	pthread_create(&thread1,NULL,(void*)time_pass,NULL);
	pthread_create(&thread2,NULL,(void*)operate,NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);	

	endwin();
	
}

void time_pass()
{
	int i;
	while(1)
	{
		//for(i=0;i<SPEED;i++);
		sleep(1);
		if(!move_brick(MOVE_DOWN))
		{
			new_brick();
			delete();
		}
		print_board();
	}
}

void operate()
{
	int op;
	while(1)
	{
		op=getch();
		switch(op)
		{
			case KEY_UP:
			change(cur_brick,&board);
			break;
			case KEY_DOWN:
			if(!move_brick(MOVE_DOWN))
			{
				new_brick();
				delete();
			}
			break;
			case KEY_LEFT:
			move_brick(MOVE_LEFT);
			break;
			case KEY_RIGHT:
			move_brick(MOVE_RIGHT);
			break;
		}
		print_board();
	}
}

void init_board()
{
	int i,j;
	for(i=0;i<BOARDHEIGHT+3;i++)
	{
		for(j=0;j<BOARDWIDTH+6;j++)
		{
			if(j<3 || j>=BOARDWIDTH+3)board.blocks[i][j]=1;
			else if(i>=BOARDHEIGHT)board.blocks[i][j]=1;
			else board.blocks[i][j]=0;
			//board.blocks[i][j]=1;
		}
	}
}
void create_brick(Brick *brick)
{
	int i,j,k;
	char *code=brick_code[rand()%7];
	for(i=0; i<BRICKSIZE; i++)
		for(j=0; j<BRICKSIZE; j++)
		{
			brick->blocks[i][j]=code[i*BRICKSIZE+j]-'0';
		}
	k=rand()%4;
	while(k--)change(brick,NULL);
	brick->x=6;
	brick->y=0;
}

void change(Brick *brick,Board *board)
{
	int i,j;
	int l,m;
	char tmp[BRICKSIZE][BRICKSIZE];
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			tmp[BRICKSIZE-1-j][i]=brick->blocks[i][j];
			if(NULL!=board)
			{
				l=BRICKSIZE-1-j;
				m=i;
				if((tmp[l][m]+board->blocks[l+brick->y][m+brick->x])>1)return;
			}
		}
	}
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			brick->blocks[i][j]=tmp[i][j];
		}
	}
	
}

int move_brick(int direction)
{
	int i,j;
	int x,y;
	if(MOVE_DOWN==direction)
	{
		x=0;
		y=1;
	}
	if(MOVE_LEFT==direction)
	{
		x=-1;
		y=0;
	}
	if(MOVE_RIGHT==direction)
	{
		x=1;
		y=0;
	}
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			if((cur_brick->blocks[i][j]+board.blocks[i+cur_brick->y+y][j+cur_brick->x+x])>1)return 0;
		}
	}
	cur_brick->x+=x;
	cur_brick->y+=y;
	return 1;
}

void print_brick(char blocks[BRICKSIZE][BRICKSIZE])
{
	int i,j;
	printw("## next:##\n");
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			if(j==0)printw("#");
			if(blocks[i][j])printw("[]");
			else printw("  ");
			if(j==BRICKSIZE-1)printw("#");
		}
		printw("\n");
	}	
	printw("##########\n");
}

void new_brick()
{
	int i,j;
	char yn;
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			if(cur_brick->blocks[i][j])board.blocks[i+cur_brick->y][j+cur_brick->x]=cur_brick->blocks[i][j];
		}
	}
	free(cur_brick);
	cur_brick=next_brick;
	if(is_gameovered())
	{
		if(cur_brick)free(cur_brick);
		score=0;
		init_board();
		init_brick();
	}

	next_brick=(Brick*)(malloc(sizeof(Brick)));
	create_brick(next_brick);
}

int is_gameovered()
{
	int i,j;
	for(i=0;i<BRICKSIZE;i++)
	{
		for(j=0;j<BRICKSIZE;j++)
		{
			if(cur_brick->blocks[i][j] && board.blocks[i+cur_brick->y][j+cur_brick->x])return 1;
		}
	}
	return 0;
}

void init_brick()
{
	cur_brick=(Brick*)(malloc(sizeof(Brick)));
	next_brick=(Brick*)(malloc(sizeof(Brick)));
	create_brick(cur_brick);
	create_brick(next_brick);
}

int delete()
{
	
	int i,j;
	int count=0;
	int lines[4];
	for(i=0;i<BOARDHEIGHT;i++)
	{
		for(j=3;j<BOARDWIDTH+3;j++)
		{
			if(!board.blocks[i][j])break;
		}
		if(j==BOARDWIDTH+3)
		{
			lines[count++]=i;
		}
	}
	if(count>0)
	{
		score+=score_define[count];
		while(count)
		{
			j=lines[--count];
			for(i=0;i<count;i++)lines[i]++;
			for(i=3;i<BOARDWIDTH+3;i++)board.blocks[j][i]=0;
			for(;j>0;j--)
			{
				for(i=3;i<BOARDWIDTH+3;i++)
				{
					board.blocks[j][i]=board.blocks[j-1][i];
					board.blocks[j-1][i]=0;
				}
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void print_board()
{
	int i,j,k;
	mvprintw(0,0,"");
	printw("       score:%d\n",score);
	for(i=0;i<sizeof(board.blocks)/sizeof(board.blocks[0])-3;i++)
	{
		printw("#");
		for(j=3;j<sizeof(board.blocks[0])-3;j++)
		{
			if(i>=cur_brick->y && i<(cur_brick->y+BRICKSIZE) && j>=cur_brick->x && j<(cur_brick->x+BRICKSIZE))
			{
				if(cur_brick->blocks[i-cur_brick->y][j-cur_brick->x])
				{
					printw("[]");
					continue;
				}
			}
			if(board.blocks[i][j])printw("[]");
			else printw("  ");
		}
		printw("#");
		if(i==0)printw("##next:##");
		if(i>0 && i<BRICKSIZE+1)
		{
			for(k=0;k<BRICKSIZE;k++)
			{
				if(next_brick->blocks[i-1][k])printw("[]");
				else printw("  ");
			}
			printw("#");
		}
		if(i==BRICKSIZE+1)printw("#########");
		printw("\n");
	}
	printw("######################");
}
