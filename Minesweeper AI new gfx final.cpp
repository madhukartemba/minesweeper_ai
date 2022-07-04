#include<iostream>
#include<iomanip>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#include<string.h>
#include<math.h>
#include <bits/stdc++.h>
#include<string.h>
#include<pthread.h>
using namespace std;


int nbombs = 10;
int length=10, width=10;
int start_seed = 0;

int *mainarr;
int *userarr;
bool *flagarr;
float *probabilityarr;
bool *digarr;
bool *safe_cell_arr;

int *p1_arr;
int *p2_arr;

int buttonloc[3*4] = {0}, buttoncount = 0;
int mouseoffx=0, mouseoffy = 0;
int boardtx = 0, boardty = 0, cm=8, cell_size = 6;

int digc = 0, safe_cell_count = 0, nflags=0, page = 0;
int screenx = GetSystemMetrics(SM_CXSCREEN), screeny = GetSystemMetrics(SM_CYSCREEN);
bool calibratemouseatstart=0, run_benchmark=0, print_info=0, showanim=1, isfirstmove=1, print_cell_probability_and_more_info=0;
int ai_method=3;

bool disp_board_en = 1;

int delay_between_anim = 0;

int safe = 1;

unsigned int process_id = getpid();

int unpstyle=1, ppstyle=1, fpstyle=1, nbstyle=1;
int unpcolor=10, ppcolor=9, fpcolor=1, nbcolor=14;
int cellfont=2, cellfontsize = 1;
int buttonfont=3, buttonfontsize = 2;


POINT cpos;
struct timeval time1, timebench1, timebench2;

pthread_t exitbutton;

void prepdisplay(int anim = 0)
{
	if(disp_board_en == 0){return;}
	if(anim==1 && showanim==0)
	{
		return;
	}
	setactivepage(page);
	setvisualpage(1-page);
}

void display(int anim = 0)
{
	if(disp_board_en == 0){return;}
	if(anim==1 && showanim==0)
	{
		return;
	}
	
	page = 1 - page;
	setactivepage(page);
	setvisualpage(1-page);
	
	
}

bool check_button(int c, int x, int y)
{
	return (buttonloc[c*4]<=x && buttonloc[c*4+1]<=y && buttonloc[c*4+2]>=x && buttonloc[c*4+3]>=y);
}

int get_button_press()
{
	int bn=0;
	while(GetAsyncKeyState(VK_LBUTTON)==0 && GetAsyncKeyState(VK_RBUTTON)==0)
	{
		//delay(10);
	}
	if(GetAsyncKeyState(VK_LBUTTON)<0)
	{
		bn = 0;
	}
	else
	{
		bn = 1;
	}
	while(GetAsyncKeyState(VK_LBUTTON)<0 || GetAsyncKeyState(VK_RBUTTON)<0)
	{
		//delay(10);
	}
	GetCursorPos(&cpos);
	cpos.x += mouseoffx;
	cpos.y += mouseoffy;
	
	return bn;
}


void calibrate_mouse()
{
	char* msg = (char*)"Click top left corner of the screen to calibrate mouse.";
	prepdisplay();
	setcolor(LIGHTGREEN);
	settextstyle(buttonfont, 0, 4);
	outtextxy((screenx-textwidth(msg))/2,(screeny-textheight(msg))/2, msg);
	display();
	get_button_press();
	mouseoffx = -cpos.x;
	mouseoffy = -cpos.y;
	display();
	cleardevice();
	display();
	
}

void solid_rectangle(int x1, int y1, int x2, int y2, int color)
{
	setcolor(color);
	int i;
	for(i=x1;i<=x2;i++)
	{
		line(i, y1, i, y2);
	}
	
}


void highlight_cell(int r, int c, int color = YELLOW)
{
	if(disp_board_en == 0){return;}
	if(showanim==0)
	{
		return;
	}
	int x = cm*c+boardtx;
	int y = cm*r+boardty;
	setfillstyle(SOLID_FILL, color);
	floodfill(x+1, y+1, BLACK);
	
}

void center_board()
{
	if(disp_board_en == 0){return;}
	int xlen, ylen;
	
	xlen = (width-1)*cm + cell_size;
	ylen = (length-1)*cm + cell_size;
	
	
	while((float)xlen<(float)screenx-60 && (float)ylen<(float)screeny-60)
	{
		cell_size++;
		cm++;
		xlen = (width-1)*cm + cell_size;
		ylen = (length-1)*cm + cell_size;
		
	}
	
	if(cell_size>6)
	{
		cell_size--;
		cm--;
	}
	
	xlen = (width-1)*cm + cell_size;
	ylen = (length-1)*cm + cell_size;
	
	xlen = screenx - xlen;
	ylen = screeny - ylen;
	if(xlen>1)
	{
		boardtx = xlen/2;
	}
	else
	{
		boardtx = 0;
	}
	
	if(ylen>1)
	{
		boardty = ylen/2;
	}
	else
	{
		boardty = 0;
	}
	
}


int get_parameters()
{
	
	char ch;
	cout<<"  __  __   _                                                                                   _____                       \n |  \\/  | (_)                                                                          /\\     |_   _|                      \n | \\  / |  _   _ __     ___   ___  __      __   ___    ___   _ __     ___   _ __      /  \\      | |                        \n | |\\/| | | | | '_ \\   / _ \\ / __| \\ \\ /\\ / /  / _ \\  / _ \\ | '_ \\   / _ \\ | '__|    / /\\ \\     | |                        \n | |  | | | | | | | | |  __/ \\__ \\  \\ V  V /  |  __/ |  __/ | |_) | |  __/ | |      / ____ \\   _| |_                       \n |_|  |_| |_| |_| |_|  \\___| |___/   \\_/\\_/    \\___|  \\___| | .__/   \\___| |_|     /_/    \\_\\ |_____|                      \n                                                            | |                                                            \n                                                            |_|                                                            \n  ____              __  __               _   _               _                      _______                  _             \n |  _ \\            |  \\/  |             | | | |             | |                    |__   __|                | |            \n | |_) |  _   _    | \\  / |   __ _    __| | | |__    _   _  | | __   __ _   _ __      | |  ___   _ __ ___   | |__     __ _ \n |  _ <  | | | |   | |\\/| |  / _` |  / _` | | '_ \\  | | | | | |/ /  / _` | | '__|     | | / _ \\ | '_ ` _ \\  | '_ \\   / _` |\n | |_) | | |_| |   | |  | | | (_| | | (_| | | | | | | |_| | |   <  | (_| | | |        | ||  __/ | | | | | | | |_) | | (_| |\n |____/   \\__, |   |_|  |_|  \\__,_|  \\__,_| |_| |_|  \\__,_| |_|\\_\\  \\__,_| |_|        |_| \\___| |_| |_| |_| |_.__/   \\__,_|\n           __/ |                                                                                                           \n          |___/                                                                                                            ";
	
	cout<<"\n\nEnter e for easy, m for medium and h for hard difficulty preset.\nEnter any other key for a custom board.";
	ch = _getch();
	
	if(ch=='e' || ch=='E')
	{
		length = 10;
		width = 10;
		nbombs = 10;
		center_board();
		return 0;
	}
	if(ch=='m'|| ch=='M')
	{
		length = 16;
		width = 16;
		nbombs = 40;
		center_board();
		return 0;
	}
	if(ch=='h'|| ch=='H')
	{
		length = 16;
		width = 30;
		nbombs = 99;
		center_board();
		return 0;
	}
	
	cout<<"\n\nEnter the length of board: ";
	cin>>length;
	cout<<"Enter the width of board: ";
	cin>>width;
	length = max(1, length);
	width = max(1, width);
	
	while(length*width<10)
	{
		cout<<"\nThere should atleast be 10 cells";
		cout<<"\n\nEnter the length of board: ";
		cin>>length;
		cout<<"Enter the width of board: ";
		cin>>width;
		length = max(1, length);
		width = max(1, width);
	}
	
	center_board();
	
	cout<<"Enter the number of mines: ";
	cin>>nbombs;
	while(nbombs>length*width-9)
	{
		cout<<"Number of mines should be less than or equal to "<<length*width-9<<"\n";
		cout<<"Enter the number of bombs: ";
		cin>>nbombs;
	}
	
	nbombs = max(0, nbombs);
	int i=0, j=0, k=0;
	
	cout<<"Do you want to run a benchmark? ";
	cin>>i;                                  
	
	if(i==1)
	{
		run_benchmark=1;
		cout<<"How many games should the AI play? ";
		cin>>j;
		j = max(1, j);
		
		cout<<"Do you want to display the board? ";
		cin>>i;
		if(i==0)
		{
			disp_board_en = 0;
		}
	}
	
	cout<<"Do you want to change settings? ";
	cin>>i;
	if(i==1)
	{
		cout<<"Select the method to be used (1=>arb_prob_solve, 2=>perimeter_solve, 3=>chunk_solve(recommended)) (default is 3): ";
		cin>>ai_method;
		ai_method = min(3, max(1, ai_method));
		
		if(ai_method==3)
		{
			cout<<"Do you want to print processing information?: ";
			cin>>i;
			if(i==1)
			{
				print_info=1;
				print_cell_probability_and_more_info=1;
			}
		}
		
		
		if(ai_method!=1)
		{
			if(print_cell_probability_and_more_info==0)
			{
				cout<<"Do you want to print the probabilities of each cell of being a mine and other info?: ";
				cin>>i;
				if(i==1)
				{
					print_cell_probability_and_more_info=1;
				}
			}
			if(disp_board_en == 1)
			{
				cout<<"Do you want to show animations? ";
				cin>>i;
				if(i==0)
				{
					showanim=0;
				}
				
				cout<<"Enter the delay between each frame (ms): ";
				cin>>i;
				delay_between_anim = max(0,i);
			}
		}
		
		
		if(disp_board_en==1)
		{
			cout<<"Do you want to change board position? ";
			cin>>i;
			if(i==1)
			{
				cout<<"Enter the board starting X position: ";
				cin>>boardtx;
				cout<<"Enter the board starting Y position: ";
				cin>>boardty;
			}
		}
		
		if(run_benchmark==0)
		{
			cout<<"Enter the seed value(0 for automatic selection): ";
			cin>>start_seed;
			start_seed = max(0, start_seed);
			
			cout<<"Do you want to calibrate the mouse cursor? ";
			cin>>i;
			if(i==1)
			{
				calibratemouseatstart=1;
			}
			
		}
		
		
	}
	
	if(run_benchmark==1)
	{
		return j;
	}
	else
	{
		return 0;
	}
}



void createbutton(char* val,int tx, int ty, int sno, int offx=0, int anim = 0, float pcomplete = -1)
{
	if(disp_board_en == 0){return;}
	
	setcolor(LIGHTGREEN);
	settextstyle(buttonfont, 0, buttonfontsize);
	int rectanglelen = textwidth(val)+10, rectangleh = textheight(val)+5;
	if(anim==0)
	{
		rectangle(tx, ty, rectanglelen+tx, rectangleh+ty);
		solid_rectangle(tx+1, ty+1, rectanglelen+tx-1, rectangleh+ty-1, BLACK);
		
		setcolor(LIGHTGREEN);
		outtextxy(tx+(rectanglelen-textwidth(val))/2+offx, ty+(rectangleh-textheight(val))/2, val);
		
		
		if(sno>=buttoncount)
		{
			buttonloc[buttoncount*4] = tx;
			buttonloc[buttoncount*4+1] = ty;
			buttonloc[buttoncount*4+2] = rectanglelen+tx;
			buttonloc[buttoncount*4+3] = rectangleh+ty;
			buttoncount++;
		}
	}
	
	if(pcomplete>=0 && anim==1)
	{
		solid_rectangle(tx+3, ty+rectangleh-3, tx+3 + pcomplete*(rectanglelen-6), ty+rectangleh-2, YELLOW);
		if(pcomplete<1)
		{
			solid_rectangle(tx+3 + pcomplete*(rectanglelen-6)+1, ty+rectangleh-3, tx+rectanglelen-3, ty+rectangleh-2, BLACK);
		}
	}
	
	
	setcolor(WHITE);
	
}

void initialise_arrays()
{
	
	mainarr = new int[length*width];
	userarr = new int[length*width];
	p1_arr = new int[length*width];
	p2_arr = new int[length*width];
	flagarr = new bool[length*width];
	probabilityarr = new float[length*width];
	digarr = new bool[length*width];
	safe_cell_arr = new bool[length*width];
	
	
}

void resetarrays()
{
	int i, j, k;
	for(i=0;i<length;i++)
	{
		k = i*width;
		for(j=0;j<width;j++)
		{
			mainarr[k+j] = 0;
			userarr[k+j] = 0;
			flagarr[k+j] = 0;
			p1_arr[k+j] = -10;
			p2_arr[k+j] = -10;
			probabilityarr[k+j] = -1;
			digarr[k+j] = 0;
			safe_cell_arr[k+j] = 0;
			
		}
	}
	digc = 0;
	safe_cell_count = 0;
	nflags=0;
	safe = 1;
}


void add_to_safe(int r, int c)
{
	if(safe_cell_arr[r*width+c] == 0)
	{
		safe_cell_arr[r*width+c] = 1;
		safe_cell_count++;
	}
}

void remove_from_safe(int x, int y)
{
	if(safe_cell_arr[x*width+y] == 1)
	{
		safe_cell_arr[x*width+y] = 0;
		safe_cell_count--;	
	}
	
}



void flagcell(int r, int c)
{
	if(flagarr[r*width+c] == 0)
	{
		flagarr[r*width+c] = 1;
		nflags++;
	}
	
}

void unflagcell(int r, int c)
{
	if(flagarr[r*width+c]==1)
	{
		flagarr[r*width+c] = 0;
		nflags--;
	}
	 
}

int is_dug(int r, int c)
{
	if(digarr[r*width+c]==1)
	{
		return 1;
	}
	
	return 0;
	
}


void dig_add(int r, int c)
{
	if(digarr[r*width+c] == 0)
	{
		digarr[r*width+c] = 1;
		digc++;
	}
	
}


int dig(int r, int c)
{
	dig_add(r, c);
	
	if(mainarr[r*width+c] == -1)
	{
		safe = 0;
		return 0;
	}
	
	if(mainarr[r*width+c] > 0)
	{
		return 1;
	}
	
	int i=0, j=0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(is_dug(i,j)==1)
			{
				continue;			
			}
			
			dig(i, j);
		}
	}
	
	return 1;
}


void print(int *arr)
{
//	if(print_info==0)
//	{
//		return;
//	}
	int i=0,j=0;
	cout<<setw(5);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(arr[i*width+j]<-1)
			{
				cout<<" "<<setw(5);
			}
			else
			{
				cout<<arr[i*width+j]<<setw(5);
			}
		}
		cout<<endl<<endl;
	}
}

int get_number_of_bombs(int x, int y)
{
	int i=0, j=0, val=0;
	for(i = max(0, x-1); i < min(length-1, x+1)+1; i++)
	{
		for(j = max(0, y-1); j < min(width-1, y+1)+1; j++)
		{
			if(!(i==x && j==y))
			{
				if(mainarr[i*width+j] == -1)
				{
					val+=1;
				}
			}
		}
	}
	return val;
}

unsigned int hash3(unsigned int h1, unsigned int h2, unsigned int h3)
{
	return ((h1*2654435789U+h2)*2654435789U) + h3;
}

void createfixedmines(int r=-1, int c=-1)
{
	int i=0, j=0, k=0;
	if(start_seed <= 0)
	{
		mingw_gettimeofday(&time1,NULL);
		//srand(time1.tv_usec + time1.tv_sec - 1623675500);
		unsigned int val=hash3(time1.tv_sec, time1.tv_usec, process_id);
		srand(val);
		cout<<"\nThe seed is: "<<val<<"\n\n";
	}
	else
	{
		srand(start_seed);
	}
	while(k<nbombs)
	{
		i = rand()%length;
		j = rand()%width;
		
		if(start_seed <= 0)
		{
			if((i<=r+1 && i>=r-1) && (j<=c+1 && j>=c-1))
			{
				continue;
			}
		}
		
		if(mainarr[i*width+j] != -1)
		{
			mainarr[i*width+j] = -1;
			k++;
		}
	}
	
	start_seed = 0;
	
}


void assignvalues()
{
	int i=0, j=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(mainarr[i*width+j] == -1)
			{
				continue;
			}
			mainarr[i*width+j] = get_number_of_bombs(i, j);
		}
	}
}


int min(int x, int y)
{
	if(x<y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

int max(int x, int y)
{
	if(x>y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

void update_user_board()
{
	int i=0, j=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(flagarr[i*width+j] == 0)
			{
				if(is_dug(i, j)==1)
				{
					userarr[i*width+j] = mainarr[i*width+j];
				}
				else
				{
					userarr[i*width+j] = -2;
				}
			}
			else
			{
				userarr[i*width+j] = -1;
			}
			
		}
	}
}



void* exit_button_check(void *val)
{
	while(1)
	{
		if(buttoncount>0)
		{
			while(1)
			{
				get_button_press();
				if(check_button(0,cpos.x,cpos.y) == 1)
				{
					exit(0);
				}
			}
		}
	}
}

void initdisplay()
{
	if(disp_board_en == 0){return;}
	initwindow(screenx, screeny, "", -3, -3);
	
	settextstyle(cellfont, 0, cellfontsize);
	
	while(textwidth((char*)"1")<cell_size && textheight((char*)"1")<cell_size)
	{
		cellfontsize++;
		if(cellfont == 2 && cellfontsize>7)
		{
			cellfont = 3;
			cellfontsize = 1;
		}
		settextstyle(cellfont, 0, cellfontsize);
	}
	
	if(cellfontsize>1 && cellfont == 2)
	{
		
		if(cellfontsize>6)
		{
			cellfontsize-=2;
		}
		cellfontsize--;
	}
	
	if(cellfontsize > 3 && cellfont == 3)
	{
		cellfontsize-=3;
	}
	if(cellfontsize>20)
	{
		cellfontsize = 20;
	}
	
	if(calibratemouseatstart==1)
	{
		calibrate_mouse();
	}
	
	cleardevice();
	prepdisplay();
	createbutton((char*)"Exit", max(min(boardtx+width*cm, screenx)-50, 0), min(boardty+length*cm+3, screeny-30), 0, 0);
	display();
	
	
	pthread_create(&exitbutton, NULL, &exit_button_check, (void*)1);
	
}



void makecell(int x, int y, int val, bool clearcell)
{
	if(disp_board_en == 0){return;}
	int i=0, j=0;
	
	if(clearcell==1)
	{
		solid_rectangle(x, y, x+cell_size, y+cell_size, BLACK);
	}
	
	
	setcolor(WHITE);
	rectangle(x,y,cell_size+x, cell_size+y);
	
	if(val==-2)
	{
		//covered cell
//		setfillstyle(SOLID_FILL, WHITE);
//		floodfill(x+1, y+1, WHITE);
		solid_rectangle(x, y, x+cell_size-2, y+cell_size-2,WHITE);
		return;
	}
	
	
	if(val==0)
	{
		//cell numbered 0
//		setfillstyle(SOLID_FILL, CYAN);
//		floodfill(x+1, y+1, WHITE);
		solid_rectangle(x+1, y+1, x+cell_size-1, y+cell_size-1, CYAN);
		return;
	}
	
	
	if(val==-1)
	{
		//flagged cell
		setcolor(YELLOW);
		outtextxy(x+(cell_size - textwidth((char*)"F"))/2, y+(cell_size - textheight((char*)"F"))/2, (char*)"F");
		return;
	}
	
	if(val>0)
	{
		//numbered cells from 1 to 8
		char c[1];
		sprintf(c, "%d", val);
		outtextxy(x+(cell_size - textwidth(c))/2, y+(cell_size - textheight(c))/2, c);
		return;
	}
	
	
}


void mark_to_update(int i, int j)
{
	if(page == 0)
	{
		p1_arr[i*width+j] = -9;
	}
	else
	{
		p2_arr[i*width+j] = -9;
	}
}

void displayboard(int anim = 0, float pcomplete = -1)
{
	if(disp_board_en == 0){return;}
	if(anim==1 && showanim==0)
	{
		return;
	}
	
	//cleardevice();
	int i, j;
	settextstyle(cellfont, 0, cellfontsize);
	
	if(buttoncount>2)
	{
		solid_rectangle(buttonloc[2*4], buttonloc[2*4+1], buttonloc[2*4+2], buttonloc[2*4+3], BLACK);
	}
	
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(page == 0)
			{
				if(p1_arr[i*width+j] == userarr[i*width+j])
				{
					continue;
				}
				else
				{
					
					makecell(cm*j+boardtx,cm*i+boardty, userarr[i*width+j], p1_arr[i*width+j] != -10);
					p1_arr[i*width+j] = userarr[i*width+j];
				}
			}
			else
			{
				if(p2_arr[i*width+j] == userarr[i*width+j])
				{
					continue;
				}
				else
				{
					makecell(cm*j+boardtx,cm*i+boardty, userarr[i*width+j], p2_arr[i*width+j] != -10);
					p2_arr[i*width+j] = userarr[i*width+j];
				}
			}
		}
	}
	
	setcolor(LIGHTGREEN);
	settextstyle(buttonfont, 0, buttonfontsize);
	
	createbutton((char*)"Exit", max(min(boardtx+width*cm, screenx)-50, 0), min(boardty+length*cm+3, screeny-30), 0, 0, anim);
	
	if(run_benchmark==0 || showanim==1)
	{
		createbutton((char*)"AI Solve", max(min(boardtx+width*cm, screenx)-200, 0), min(boardty+length*cm+3, screeny-30), 1, 1, anim, pcomplete);
		//createbutton((char*)"AI Solve", max(min(boardtx+width*cm, screenx)-150, 0), min(boardty+length*cm+3, screeny-25), 0, 0, anim, pcomplete);
	}
	
	const char* nminesleft = to_string(nbombs-nflags).c_str();
	char minelstr[12+strlen(nminesleft)] = "Mines left: ";
	strcat(minelstr, nminesleft);
	createbutton(minelstr, max(buttonloc[1*4]-textwidth(minelstr)-30, 0), min(boardty+length*cm+3, screeny-30), 2, 0);
//	outtextxy(max(buttonloc[0]-textwidth(minelstr)-30, 0), min(boardty+length*cm+7, screeny-27), minelstr);
}


void get_user_input()
{
	int val=0, x, y;
	cout<<"\nFLAG, UNFLAG, DIG? (0,1,2) ";
	cin>>val;
	cout<<"\nENTER THE ROW: ";
	cin>>x;
	cout<<"\nENTER THE COLUMN: ";
	cin>>y;
	
	while(x<0 || y<0 || x>=length || y>=width)
	{
		cout<<"\nINVALID LOCATION\n";
		cout<<"\nENTER THE ROW: ";
		cin>>x;
		cout<<"\nENTER THE COLUMN: ";
		cin>>y;
	}
	
	cout<<"\nSelected Location is ("<<x<<","<<y<<")\n";
	if(val==0)
	{
		flagcell(x, y);
	}
	if(val==1)
	{
		unflagcell(x, y);
	}
	if(val==2)
	{
		dig(x, y);
	}
	

}


void change_cell_prob(int x, int y, float p)
{
	if(probabilityarr[x*width+y]<0)
	{
		probabilityarr[x*width+y] = 1;
	}
	probabilityarr[x*width+y] = probabilityarr[x*width+y] + p;
}




void select_random_cell(int &r, int &c)
{
	int i = 0;
	if(userarr[0]==-2)
	{
		r = 0;
		c = 0;
		return;
	}
	
	if(userarr[(length-1)*width]==-2)
	{
		r = length-1;
		c = 0;
		return;
	}
	
	if(userarr[width-1]==-2)
	{
		r = 0;
		c = width-1;
		return;
	}
	
	if(userarr[(length-1)*width + width-1]==-2)
	{
		r = length-1;
		c = width-1;
		return;
	}
	
	
	for(i=1;i<width-1;i++)
	{
		if(userarr[(length-1)*width + i]==-2)
		{
			r = length-1;
			c = i;
			return;
		}
		
		if(userarr[i]==-2)
		{
			r = 0;
			c = i;
			return;
		}
		
	}
	
	for(i=1;i<length-1;i++)
	{
		if(userarr[i*width]==-2)
		{
			r = i;
			c = 0;
			return;
		}
		
		if(userarr[i*width + width-1]==-2)
		{
			r = i;
			c = width-1;
			return;
		}
		
	}
	
	int j=0;
	
	for(i=1;i<length-1;i++)
	{
		for(j=1;j<width-1;j++)
		{
			if(userarr[i*width+j]==-2)
			{
				r = i;
				c = j;
				return;
			}
		}
	}
	cout<<"\nError in select_random_cell, no cell is left to select.";
	while(1)
	{
		delay(100);
	}
	
}


void reset_probabilityarr()
{
	int i=0,j=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			probabilityarr[i*width+j] = -1;
		}
	}
}

void select_min_prob()
{
	int i=0,j=0,r,c,counter=0, x=0;
	float min = 10;
	
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(probabilityarr[i*width+j]<0)
			{
				counter++;
			}
			else
			{
				if(probabilityarr[i*width+j]<min)
				{
					r = i;
					c = j;
					min = probabilityarr[i*width+j];
				}
			}
		}
	}
	
	x = length*width;
	
	if(counter==x)
	{
		mingw_gettimeofday(&time1, NULL); srand(time1.tv_usec);
		select_random_cell(r, c);
	}
	
	
	if(isfirstmove==1)
	{
		isfirstmove = 0;
		createfixedmines(r, c);
		assignvalues();
		reset_probabilityarr();
		dig(r, c);
	}
	else
	{
		dig(r, c);
	}
	reset_probabilityarr();
	
}

void calculate_cell_prob(int x, int y)
{
	int i = 0, j = 0, covered_cell_count=0, mine_count=0;
	int arr[8][2];
	for(i = max(0, x-1); i < min(length-1, x+1)+1; i++)
	{
		for(j = max(0, y-1); j < min(width-1, y+1)+1; j++)
		{
			if(i==x && j==y)
			{
				continue;
			}
			if(userarr[i*width+j]==-2)
			{
				arr[covered_cell_count][0] = i;
				arr[covered_cell_count][1] = j;
				covered_cell_count++;
			}
			if(userarr[i*width+j]==-1)
			{
				mine_count++;
			}
			
		}
	}
	
	if(covered_cell_count==0)
	{
		return;
	}
	float p = 0;
	j = userarr[x*width+y]-mine_count;
	if(j<0)
	{
		cout<<"Probability error!\n";
	}
	p = (float)(j)/(float)covered_cell_count;
	for(i=0;i<covered_cell_count;i++)
	{
		if(j==0)
		{
			add_to_safe(arr[i][0], arr[i][1]);
		}
		else
		{
			if(p==1)
			{
				flagcell(arr[i][0], arr[i][1]);
			}
			else
			{
				change_cell_prob(arr[i][0], arr[i][1], p);
			}
		}
	}
	
	
	
}

void find_mine_prob()
{
	int i=0, j=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(userarr[i*width+j]>0)
			{
				calculate_cell_prob(i, j);
			}
		}
	}
}

void make_decision()
{
	if(safe_cell_count>0)
	{
		int i, j;
		for(i=0;i<length;i++)
		{
			for(j=0;j<width;j++)
			{
				if(safe_cell_arr[i*width+j]==1)
				{
					dig(i, j);
					remove_from_safe(i, j);
				}
			}
		}
	}
	else
	{
		select_min_prob();
	}
}


int in_array2D(int x, int y,const int arr[], int c, int dimy)
{
	int i=0, j=0;
	for(i=0;i<c;i++)
	{
		if(arr[i*dimy+0] == x && arr[i*dimy+1] == y)
		{
			return i;
		}
	}
	return -1;
}


void get_neightboring_covered_cells(int x, int y, int arr[], int &ac)
{
	int i=0, j=0;
	for(i = max(0, x-1); i < min(length-1, x+1)+1; i++)
	{
		for(j = max(0, y-1); j < min(width-1, y+1)+1; j++)
		{
			if(i==x && j==y)
			{
				continue;
			}
			else
			{
				if(userarr[i*width+j] == -2 && flagarr[i*width+j]!=1 && in_array2D(i, j, arr, ac, 2)==-1)
				{
					arr[ac*2] = i;
					arr[ac*2+1] = j;
					ac++;
				}
			}
		}
	}
}

bool int_to_binary(int x, bool arr[], int counter)
{
	int i = 0, val=0;
	for(i=counter-1;i>=0;i--)
	{
		arr[i] = x%2;
		x = x/2;
		if(arr[i]==1)
		{
			val++;
			if(val>nbombs-nflags)
			{
				return 0;
			}
		}
	}
	return 1;
	
	
}


int is_mine(int x, int y, const bool c[], int perimeter[], int pc)
{
	int i=0, j=0;
	i = in_array2D(x,y,perimeter,pc, 2);
	if(i!=-1)
	{
		if(c[i]==0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		cout<<"\nError the point ("<<x<<","<<y<<") is not in the array\n";
		return -1;
	}
}


int check_comb(int x, int y, bool combination[], int perimeter[],int pc)
{
	int i=0,j=0,val=0, t = 0;
	for(i = max(0, x-1); i < min(length-1, x+1)+1; i++)
	{
		for(j = max(0, y-1); j < min(width-1, y+1)+1; j++)
		{
			if(i==x && j==y)
			{
				continue;
			}
			else
			{
				if(userarr[i*width+j]==-1)
				{
					val++;
				}
				else
				{
					if(userarr[i*width+j]==-2)
					{
						t = is_mine(i, j, combination, perimeter, pc);
						if(t==-1)
						{
							cout<<"\nError! value does not exist in the array perimeter!\n";
						}
						if(t==1)
						{
							val++;
						}
					}
				}
				
			}
		}
	}
	
	if(val==userarr[x*width+y])
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

int is_combination_valid(bool combination[], int perimeter[],int pc)
{
	int i=0,j=0, k=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(userarr[i*width+j]>0)
			{
				k = check_comb(i, j, combination, perimeter, pc);
				if(k==0)
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

int remaining_covered_cells(int pc=0)
{
	return length*width-(digc+nflags+pc);
}

void make_decision(const float parr[], const int perimeter[], const int pc)
{
	if(pc==0)
	{
		int x, y;
		mingw_gettimeofday(&time1, NULL); srand(time1.tv_usec);
		//select_random_cell(x, y);
		do
		{
			x = rand()%length;
			y = rand()%width;
			
		}while(userarr[x*width+y]>-2);
		
		if(print_cell_probability_and_more_info){cout<<"Randomly selected the cell ("<<x<<","<<y<<")\n";}
		if(isfirstmove==1)
		{
			isfirstmove = 0;
			createfixedmines(x, y);
			assignvalues();
			reset_probabilityarr();
			dig(x, y);
		}
		else
		{
			dig(x, y);
		}
		return;
	}
	int i=0, j=0,val = 0;
	float minval = 2;
	for(i=0;i<pc;i++)
	{
		if(parr[i]==1)
		{
			flagcell(perimeter[i*2], perimeter[i*2+1]);
		}
		if(parr[i]==0)
		{
			add_to_safe(perimeter[i*2], perimeter[i*2+1]);
		}
		if(parr[i]<minval)
		{
			minval = parr[i];
		}
	}
	if(showanim==1)
	{
		for(i=0;i<pc;i++)
		{
			if(parr[i]>=minval && parr[i]!=0)
			{
				p2_arr[perimeter[i*2]*width+perimeter[i*2+1]] = -9;
				p1_arr[perimeter[i*2]*width+perimeter[i*2+1]] = -9;
			}
				
		}
	}
	
	if(print_cell_probability_and_more_info){cout<<"Minimum mine probability is: "<<minval<<endl;}
	
	if(safe_cell_count>0)
	{
		for(i=0;i<length;i++)
		{
			for(j=0;j<width;j++)
			{
				if(safe_cell_arr[i*width+j]==1)
				{
					if(print_cell_probability_and_more_info){cout<<"\nSelected the cell ("<<i<<","<<j<<") with 0 probability of being a mine.\n";}
					dig(i, j);
					remove_from_safe(i, j);
				}
			}
		}
	}
	else
	{
		mingw_gettimeofday(&time1, NULL); srand(time1.tv_usec);
		int val=0;
		for(i=0;i<pc;i++)
		{
			if(parr[i]==minval)
			{
				val++;
			}
			
		}
		int points_arr[val][2];
		int counter=0;
		for(i=0;i<pc;i++)
		{
			if(parr[i]==minval)
			{
				points_arr[counter][0] = perimeter[i*2];
				points_arr[counter][1] = perimeter[i*2+1];
				counter++;
			}
		}
		i = remaining_covered_cells(pc);
		float rem_cell_prob;
		if(i>0)
		{
			rem_cell_prob = (float)(nbombs-nflags)/i;
		}
		else
		{
			rem_cell_prob = 2;
		}
		if(minval>rem_cell_prob)
		{
			i=0;
			j=0;
			val = 0;
			if(print_cell_probability_and_more_info){cout<<"\nIt is better to select a non-perimeter cell.";}
			while(1)
			{
				select_random_cell(i, j);
				if(userarr[i*width+j]==-2 && in_array2D(i, j, perimeter, pc, 2)==-1)
				{
					goto digging;
				}
				val++;
				if(val>100)
				{
					break;
				}
			}
			
			for(i=0;i<length;i++)
			{
				for(j=0;j<width;j++)
				{
					if(userarr[i*width+j]==-2 && in_array2D(i, j, perimeter, pc, 2)==-1)
					{
						goto digging;
					}
				}
			}
			
			digging:
				if(print_cell_probability_and_more_info){cout<<"\nSelected the non-perimeter cell ("<<i<<","<<j<<") with "<<rem_cell_prob<<" probability of being a mine.\n";}
				if(isfirstmove==1)
				{
					isfirstmove = 0;
					createfixedmines(i, j);
					assignvalues();
					reset_probabilityarr();
					dig(i, j);
				}
				else
				{
					dig(i, j);
				}
		}
		else
		{
			i = rand()%counter;
			if(print_cell_probability_and_more_info){cout<<"\nSelected the cell ("<<points_arr[i][0]<<","<<points_arr[i][1]<<") with "<<minval<<" probability of being a mine.\n";}
			if(isfirstmove==1)
			{
				isfirstmove = 0;
				createfixedmines(points_arr[i][0], points_arr[i][1]);
				assignvalues();
				reset_probabilityarr();
				dig(points_arr[i][0], points_arr[i][1]);
			}
			else
			{
				dig(points_arr[i][0], points_arr[i][1]);
			}
		}
		
	}
	
}

unsigned int nCr(int n, int k)
{
    unsigned int C[n + 1][k + 1];
    int i, j;
 
    // Caculate value of Binomial Coefficient
    // in bottom up manner
    for (i = 0; i <= n; i++) {
        for (j = 0; j <= min(i, k); j++) {
            // Base Cases
            if (j == 0 || j == i)
                C[i][j] = 1;
 
            // Calculate value using previously
            // stored values
            else
                C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
        }
    }
 
    return C[n][k];
}


void print_table(int tableno, int table[], int tablecount, int tablerowsize, int tablesize)
{
	if(print_info==0)
	{
		return;
	}
	int i=0, j=0;
	cout<<"\n Table "<<tableno;
	cout<<endl;
	for(i=0;i<tablecount;i++)
	{
		for(j=0;j<tablerowsize;j++)
		{
			cout<<table[i*tablerowsize+j]<<" ";
		}
		cout<<endl;
	}
	
}

unsigned int all_combs(int n, int r)
{
	int i=0;
	unsigned int val = 0;
	for(i=0;i<=r;i++)
	{
		val += nCr(n,i);
	}
	return val;
}


void arb_prob_solve() //Method 1
{
	find_mine_prob();
	make_decision();
}

void perimeter_anim(float parr[], int perimeter[], int &pc, int validcombs, float &pcomplete)
{
	if(disp_board_en == 0){return;}
	if(showanim==0 || validcombs==0)
	{
		return;
	}
	
	
	int i=0;
	float prob = 0;
	prepdisplay(1);
	displayboard(1, pcomplete);
	
	for(i=0;i<pc;i++)
	{
		prob = parr[i]/validcombs;
		if(prob<0.25)
		{
			highlight_cell(perimeter[i*2], perimeter[i*2+1], GREEN);
			continue;
		}
		if(prob<0.5)
		{
			highlight_cell(perimeter[i*2], perimeter[i*2+1], YELLOW);
			continue;
		}
		if(prob<0.75)
		{
			highlight_cell(perimeter[i*2], perimeter[i*2+1], LIGHTRED);
			continue;
		}
		if(prob<=1)
		{
			highlight_cell(perimeter[i*2], perimeter[i*2+1], RED);
			continue;
		}
	}
	display(1);
	
}

void perimeter_solve()  //Method 2
{
	
	int perimeter[length*width*2] = {0};
	int pc=0;
	int i=0,j=0,k=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(userarr[i*width+j]>0)
			{
				get_neightboring_covered_cells(i, j, perimeter, pc);
			}
		}
	}
	if(print_cell_probability_and_more_info){cout<<"\nCovered cell count in perimeter: "<<pc<<endl;}
	
	if(pc>22)
	{
		if(print_cell_probability_and_more_info){cout<<"\nPerimeter too big! Using 1st method to solve.\n";}
		find_mine_prob();
		make_decision();
		return;
	}
	
	
	bool combination[pc]={0};
	float parr[pc] = {0};
	int valid_comb = 0;
	unsigned int combn = 0, cn=0;
	i = nbombs-nflags;
	if(i<=pc)
	{
		combn = all_combs(pc, i);
	}
	else
	{
		combn = all_combs(pc,pc);
	}
	if(print_cell_probability_and_more_info){cout<<"\nThe number of combinations are: "<<combn<<endl;}
	
	int limit = min(nbombs-nflags, pc);
	float pcomplete = 0;
	
	for(k=0;k<=limit;k++)
	{
		
		for(i=0;i<pc;i++)
		{
			combination[i] = 0;
		}
		i=pc-1;
		j = 0;
		while(j<k)
		{
			combination[i] = 1;
			i--;
			j++;
		}
		do{
			if(is_combination_valid(combination, perimeter, pc)==1)
			{
				for(i=0;i<pc;i++)
				{
					parr[i] += combination[i];
				}
				
				valid_comb++;
				
			}
			
		}while(next_permutation(combination, combination+pc));
		pcomplete=(float)k/limit;
		if(print_cell_probability_and_more_info){cout<<pcomplete*100<<"% complete\n";}
		perimeter_anim(parr, perimeter, pc, valid_comb, pcomplete);
		delay(delay_between_anim);
				
	}
	
	if(print_cell_probability_and_more_info){cout<<"\nThe number of valid combinations are: "<<valid_comb<<endl;}
	
	if(print_cell_probability_and_more_info){cout<<"The mine probabilties are: ";}
	for(i=0;i<pc;i++)
	{
		mark_to_update(perimeter[i*2], perimeter[i*2+1]);
		parr[i] /= (float)valid_comb;
		if(print_cell_probability_and_more_info){cout<<"("<<perimeter[i*2]<<","<<perimeter[i*2+1]<<" = "<<parr[i]<<") ";}
	}
	
	
	make_decision(parr, perimeter, pc);
	
}

int number_of_cells_processed(int val ,int cellstate[], int pc)
{
	int num = 0;
	for(int i=0;i<pc;i++)
	{
		if(cellstate[i]==val)
		{
			num++;
		}
	}
	
	return num;
}

void select_next_cell(int &r, int &c, int cellstate[], int perimeter[], int pc)
{
	if(print_info){cout<<"\nIn select_next_cell.";};
	int i=0;
	for(i=0;i<pc;i++)
	{
		if(cellstate[i] == 0)
		{
			r = perimeter[i*2];
			c = perimeter[i*2+1];
			return;
		}
	}
	cout<<"\nError in select next cell! All cells are processed!\n";
	while(1);
	
}

void add_to_nbcells(int r, int c, int nbcells[], int &nbcellcount)
{
	int i=0, j=0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(userarr[i*width+j] == -2 && in_array2D(i, j, nbcells, nbcellcount, 2)==-1)
			{
				if(nbcellcount>=20)
				{
					cout<<"\n Error! nbcells array full!";
					while(1);
				}
				nbcells[nbcellcount*2] = i;
				nbcells[nbcellcount*2+1] = j;
				nbcellcount++;
				if(print_info){cout<<"\nNeighboring cell ("<<i<<","<<j<<") is added.";}
				highlight_cell(i, j);
			}
		}
	}
}

void get_nbcells(int r, int c, int nbcells[], int &nbcellcount)
{
	if(print_info){cout<<"\nIn get_nbcells.";};
	int i=0, j=0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(userarr[i*width + j] >=  0)
			{
				add_to_nbcells(i, j, nbcells, nbcellcount);
			}
		}
	}
	
}

void change_cell_state(int val, int nbcells[], int nbcellcount, int cellstate[], int perimeter[], int pc)
{
	if(print_info){cout<<"\nIn change_cell_state.";};
	int i=0, j=0;
	for(i=0;i<nbcellcount;i++)
	{
		j = in_array2D(nbcells[i*2], nbcells[i*2+1], perimeter, pc, 2);
		if(j>=0)
		{
			if(cellstate[j]==2)
			{
				cout<<"\nError! Neighboring cell ("<<nbcells[i*2]<<","<<nbcells[i*2+1]<<") has cell state as 2 (completed)";
				while(1);
			}
			cellstate[j] = val;
		}
		else
		{
			cout<<"\nError! Neighboring cell ("<<nbcells[i*2]<<","<<nbcells[i*2+1]<<") not in the perimeter!";
			while(1);
		}
	}
}


void find_check_cells(int r, int c, int check_cells[], int &checkcellcount)
{
	int i=0, j=0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(userarr[i*width + j] >= 0 && in_array2D(i, j, check_cells, checkcellcount, 2)==-1)
			{
				check_cells[checkcellcount*2] = i;
				check_cells[checkcellcount*2+1] = j;
				checkcellcount++;
				if(print_info){cout<<"\nAdded cell ("<<i<<","<<j<<") to check_cells.";}
			}
		}
	}
}


int is_check_cell_valid(int r, int c, bool comb[], int nbcells[], int nbcellcount)
{
	int i=0, j=0, k=0;
	int minecount=0, emptycells=0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(userarr[i*width+j]==-1)
			{
				minecount++;
			}
			else
			{
				if(userarr[i*width+j] == -2)
				{
					k = in_array2D(i, j, nbcells, nbcellcount, 2);
					if(k>=0)
					{
						if(comb[k]==1)
						{
							minecount++;
						}
					}
					else
					{
						emptycells++;
					}
					
				}
			}
			
		}	
	}
	
	if(emptycells>0)
	{
		i = userarr[r*width+c]-minecount;
		if(i<0)
		{
			return 0;
		}
		if(emptycells>=i)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(minecount==userarr[r*width+c])
		{
			return 1;
		}
		return 0;
	}
	
}

int is_local_combination_valid(bool comb[], int check_cells[], int checkcellcount, int nbcells[], int nbcellcount)
{
	int i=0,j=0;
	for(i=0;i<checkcellcount; i++)
	{
		j = j + is_check_cell_valid(check_cells[i*2], check_cells[i*2+1], comb, nbcells, nbcellcount);
	}
	
	if(j==checkcellcount)
	{
		return 1;
	}
	
	return 0;
	
}

void print_combination(bool comb[], int combsize)
{
	if(print_info==0)
	{
		return;
	}
	cout<<endl;
	for(int i =0;i<combsize;i++)
	{
		cout<<comb[i]<<" ";
	}
}

int in_array1D(int x, int pos, int arr[], int arrcount, int dimy)
{
	int i=0;
	for(i=0;i<arrcount;i++)
	{
		if(arr[i*dimy+pos] == x)
		{
			return i;
		}
	}
	return -1;
}


void add_row_to_table(int arr[], int table[], int &tablecount, int tablerowsize, int tablesize)
{
	int i=0;
	for(i=0;i<tablerowsize;i++)
	{
		table[tablecount*tablerowsize+i] = arr[i]; 
	}
	if(tablecount>=tablesize)
	{
		cout<<"Error! In add_row_to_table, Table is full! "<<tablesize;
		closegraph();
		while(1);
	}
	tablecount++;
	
}


void print_2darray(int arr[], int count, int rowsize)
{
	if(print_info==0)
	{
		return;
	}
	int i=0,j=0;
	cout<<endl;
	for(i=0;i<count;i++)
	{
		cout<<"  ("<<arr[i*rowsize]<<","<<arr[i*rowsize+1]<<")  ";
	}
}


int does_comb_fit(int tablerow_no, bool comb[], int perimeter[], int pc, int nbcells[], int nbcellcount, int table1[], int &table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	int i=0,j=0,k=0;
	int arr[nbcellcount*2] = {0};
	int c=0;
	
	
	for(i=0;i<nbcellcount;i++)
	{
		j = in_array2D(nbcells[i*2], nbcells[i*2+1], perimeter, pc, 2);
		if(j==-1)
		{
			cout<<"\nError! In does_comb_fit the nbcell ("<<nbcells[i*2]<<","<<nbcells[i*2+1]<<") does not exist in the perimeter!";
			while(1);
		}
		if(table1[tablerow_no*tablerowsize+j] == -1)
		{
			if(c>=nbcellcount)
			{
				cout<<"\nError! In does_comb_fit, array limit is reached! "<<c;
				while(1);
			}
			arr[2*c] = j;
			arr[2*c+1] = comb[i];
			c++;
			k++;
		}
		else
		{
			if(table1[tablerow_no*tablerowsize+j] == (int)comb[i])
			{
				k++;
			}
		}
	}
	if(k == nbcellcount)
	{
		int temptable[tablerowsize];
		for(i=0;i<tablerowsize;i++)
		{
			if(table1[tablerow_no*tablerowsize + i]==-1)
			{
				j=in_array1D(i, 0, arr, c, 2);
				if(j>=0)
				{
					temptable[i] = arr[2*j+1];
				}
				else
				{
					temptable[i] = -1;
				}
			}
			else
			{
				temptable[i] = table1[tablerow_no*tablerowsize + i];
			}
		}
		
		add_row_to_table(temptable, table2, table2count, tablerowsize, tablesize);
		return 1;
	}
	return 0;
}

void add_table1row_to_table2(int val, int rno, int table1[], int table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	int i=0,j=0;
	for(i=0;i<tablerowsize-val;i++)
	{
		table2[table2count*tablerowsize+i] = table1[rno*tablerowsize+i]; 
	}
	if(table2count>=tablesize)
	{
		cout<<"Error! In add_table1row_to_table2, Table 2 is full! "<<table2count<<" >= "<<tablesize;
		closegraph();
		while(1);
	}
	table2count++;
	print_table(2, table2, table2count, tablerowsize, tablesize);
	
}

void fit_to_table(bool comb[], int perimeter[], int pc, int nbcells[], int nbcellcount, int table1[], int &table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	if(print_info){cout<<"\nIn fit_to_table.";};
	int i=0,j=0;
	if(table1count == 0)
	{
		if(print_info){cout<<"\nTable 1 is empty.";}
		for(i=0;i<nbcellcount;i++)
		{
			j = in_array2D(nbcells[i*2], nbcells[i*2+1], perimeter, pc, 2);
			if(j==-1)
			{
				cout<<"\nError! In fit_to_table, the nbcell ("<<nbcells[i*2]<<","<<nbcells[i*2+1]<<") does not exist in the perimeter!";
				while(1);
			}
			if(table2count>=tablesize)
			{
				cout<<"Error! In fit_to_table, Table 2 is full! "<<tablesize;
				closegraph();
				while(1);
			}
			table2[table2count*tablerowsize+j] = comb[i];
		}
		table2count++;
	}
	else
	{
		for(i=0;i<table1count;i++)
		{
			if(does_comb_fit(i, comb, perimeter, pc, nbcells, nbcellcount, table1, table1count, table2, table2count, tablerowsize, tablesize)==1)
			{
				if(print_info){cout<<"\nCombination does fit.";}
				//(1, i, table1, table1count, table2, table2count, tablerowsize, tablesize);
			}
			else
			{
				print_2darray(perimeter, pc, 2);
				if(print_info){cout<<"\nCombination does not fit.";}
			}
		}
	}
}


void fit(int perimeter[], int pc, int nbcells[], int nbcellcount, int table1[], int &table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	if(print_info){cout<<"\nIn fit.";};
	int i=0,j=0,k=0;
	int check_cells[(nbcellcount*8)*2] = {0}; //Assuming 8 check cells for every nbcell
	int checkcellcount=0;
	
	//Get all the cells which determine if a combination is valid or not
	for(i=0;i<nbcellcount;i++)
	{
		find_check_cells(nbcells[i*2], nbcells[i*2+1], check_cells, checkcellcount);
	}
	if(print_info)
	{
		cout<<"\nCheck cell count is "<<checkcellcount;
	}
	
	
	// Now check and store all the local valid the combinations
	bool comb[nbcellcount] = {0};
	int ncombs = pow(2, nbcellcount);
	if(print_info)
	{
		cout<<"\nCheck cell count is "<<checkcellcount;
		cout<<"\nThe number of combinations are: "<<ncombs;
	}

	
	for(i=0;i<ncombs;i++)
	{
		if(int_to_binary(i, comb, nbcellcount)==1)
		{
			k = is_local_combination_valid(comb, check_cells, checkcellcount, nbcells, nbcellcount);
			if(k==1)
			{
				print_combination(comb, nbcellcount);
				j = j+1;
				fit_to_table(comb, perimeter, pc, nbcells, nbcellcount, table1, table1count, table2, table2count, tablerowsize, tablesize);
			}
		}
	}
	if(print_info)
	{
		cout<<"\nThe total number of valid combinations are: "<<j;
	}
	if(j==0)
	{
		cout<<"\nError! There are no valid combinations!";
		closegraph();
		while(1);
	}
	
}


void reset_table(int table[], int &tablecount, int tablerowsize, int tablesize)
{
	int i=0,j=0;
	for(i=0;i<tablesize;i++)
	{
		for(j=0;j<tablerowsize;j++)
		{
			if(j==tablerowsize-1)
			{
				table[i*tablerowsize+j] = 1;
			}
			else
			{
				table[i*tablerowsize+j] = -1;
			}
		}
	}
	tablecount = 0;
	//print_table(-1, table, tablecount, tablerowsize, tablesize);
}

void shift_table2_to_table1(int table1[], int &table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	reset_table(table1, table1count, tablerowsize, tablesize);
	int i=0,j=0;
	
	for(i=0;i<table2count;i++)
	{
		for(j=0;j<tablerowsize;j++)
		{
			table1[i*tablerowsize+j] = table2[i*tablerowsize+j];
		}
	}
	table1count = table2count;
	reset_table(table2, table2count, tablerowsize, tablesize);
	
}

int does_nb_contain_unpc(int r, int c, int cellstate[], int perimeter[], int pc)
{
	int i=0, j=0, k=0;
	int val = 0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(userarr[i*width+j]>=-1)
			{
				continue;
			}
			else
			{
				k = in_array2D(i, j, perimeter, pc, 2);
				if(k==-1)
				{
					
					cout<<"\nError! In does_nb_contain_unpc, the cell ("<<i<<","<<j<<") does not exist in the perimeter!";
					print_2darray(perimeter, pc, 2);
					cout<<endl<<userarr[i*width+j];
					while(1);
				}
				if(cellstate[k]==0)
				{
					return 1;
				}
			}
		}
	}
	
	return 0;
	
}

int is_cell_processed(int r, int c, int cellstate[], int perimeter[], int pc)
{
	int i=0, j=0, k=0;
	int val = 0;
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(i==r && j==c)
			{
				continue;
			}
			else
			{
				
				if(userarr[i*width+j]>=0)
				{
					if(does_nb_contain_unpc(i, j, cellstate, perimeter, pc)==1)
					{
						return 0;
					}
				}
			}
		}
	}
	
	return 1;
}


int str_match_row(int r1, int r2, int cellstate[], int table1[], int table1count, int tablerowsize, int tablesize)
{
	int i=0,j=0;
	if(table1count==0)
	{
		cout<<"\nTable empty";
		while(1);
	}
	//print_table(1, table1, table1count, tablerowsize, tablesize);
	for(i=0;i<tablerowsize-1;i++)
	{
		//if(print_info){cout<<"("<<table1[r1*tablerowsize+i]<<","<<table1[r2*tablerowsize+i]<<")"<<"  ";}
		if(cellstate[i]==2)
		{
			continue;
		}
		if(table1[r1*tablerowsize+i] != table1[r2*tablerowsize+i])
		{
			return 0;
		}
	}
	
	//if(print_info){cout<<"\nRows "<<r1<<" and "<<r2<<" do match.";}
	return 1;
}


void print_1darray(int arr[], int arrc)
{
	if(print_info==0)
	{
		return;
	}
	int i =0;
	cout<<endl;
	for(i=0;i<arrc;i++)
	{
		cout<<arr[i]<<" ";
	}
}


void combine_rows(int r1, int combarr[], int combarrcount,int cellstate[], int table[], int &tablecount, int tablerowsize, int tablesize)
{
	int i=0,j=0,k=0;
	for(i=0;i<combarrcount;i++)
	{
		for(j=0;j<tablerowsize;j++)
		{
			if(j==tablerowsize-1)
			{
				table[r1*tablerowsize+j] = table[r1*tablerowsize+j] + table[combarr[i]*tablerowsize+j];
				if(print_info)
				{
					cout<<"\nTotal: "<<table[r1*tablerowsize+j];
				}
				continue;
			}
			if(cellstate[j] == 2)
			{
				if(table[r1*tablerowsize+j]==-1)
				{
					cout<<"\nTable values are negative.";
					while(1);
				}
				table[r1*tablerowsize+j] = table[r1*tablerowsize+j] + table[combarr[i]*tablerowsize+j];
			}
		}
	}
}

void shrink_table1(int cellstate[], int perimeter[], int pc, int table1[], int &table1count, int table2[], int &table2count, int tablerowsize, int tablesize)
{
	if(print_info){cout<<"\nIn shrink_table1.";};
	int i=0,j=0,k=0;
	
	for(i=0;i<pc;i++)
	{
		if(cellstate[i] == 1)
		{
			if(is_cell_processed(perimeter[i*2], perimeter[i*2+1], cellstate, perimeter, pc)==1)
			{
				cellstate[i] = 2;
				for(j=0;j<table1count;j++)
				{
					table1[j*tablerowsize+i] = table1[j*tablerowsize+i]*table1[(j+1)*tablerowsize-1];
				}
			}
		}
	}
	
	if(print_info){cout<<"\nCell state";}
	print_1darray(cellstate, pc);
	
	print_table(1, table1, table1count, tablerowsize, tablesize);
	
	//Now lets shrink the table
	int combarr[tablesize] = {0};
	int combarrcount = 0;
	
	int alreadymatched[tablesize] = {0}, alreadymatchedcount = 0;
	
	for(i=0;i<table1count-1;i++)
	{
		if(in_array1D(i, 0, alreadymatched, alreadymatchedcount, 1)>=0)
		{
			if(print_info){cout<<"\nThe row "<<i<<" is already matched.";}
			continue;
		}
		for(j=i+1;j<table1count;j++)
		{
			if(str_match_row(i, j, cellstate, table1, table1count, tablerowsize, tablesize)==1)
			{
				if(in_array1D(i, 0, alreadymatched, alreadymatchedcount, 1)<0)
				{
					alreadymatched[alreadymatchedcount] = i;
					alreadymatchedcount++;
				}
				k++;
				if(combarrcount>=tablesize || alreadymatchedcount>=tablesize)
				{
					cout<<"\nError! In shrink_table1, an array has exceeded size limit: "<<tablesize;
					while(1);
					
				}
				combarr[combarrcount] = j;
				combarrcount++;
				alreadymatched[alreadymatchedcount] = j;
				alreadymatchedcount++;
				
				
			}
		}
		if(k>0)
		{
			combine_rows(i, combarr, combarrcount, cellstate, table1, table1count, tablerowsize, tablesize);
			add_table1row_to_table2(0, i, table1, table1count, table2, table2count, tablerowsize, tablesize);
		}
		k=0;
		combarrcount=0;
	}
	
	print_table(2, table2, table2count, tablerowsize, tablesize);
	if(table2count==0) // No matching rows
	{
		return;
	}
	
	//Lets add the remaining unmatched combinations in table1 also to table2
	for(i=0;i<table1count;i++)
	{
		if(in_array1D(i, 0, alreadymatched, alreadymatchedcount, 1)==-1)
		{
			add_table1row_to_table2(0, i, table1, table1count, table2, table2count, tablerowsize, tablesize);
		}
	}
	
	
	
	//Now lets transfer the shrinked table back to table1
	shift_table2_to_table1(table1, table1count, table2, table2count, tablerowsize, tablesize);
	//cout<<"\nTable1";
	//print_table(1, table1, table1count, tablerowsize, tablesize);
	
}

void sort_perimeter(int perimeter[], int pc)
{
	int temp_perimeter[pc*2];
	int temppc = 0;
	int i=0,j=0,k=0,r=0,c=0;
	
	for(k=0;k<pc;k++)
	{
		r = perimeter[k*2];
		c = perimeter[k*2+1];
		//cout<<"Cells in the perimeter sorted: ";
		for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
		{
			for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
			{
				if(in_array2D(i, j, perimeter, pc, 2)>=0)
				{
					if(in_array2D(i, j, temp_perimeter, temppc, 2)==-1)
					{
						//cout<<"("<<i<<","<<j<<") ";
						temp_perimeter[temppc*2]=i;
						temp_perimeter[temppc*2+1]=j;
						temppc++;
					}
					else
					{
						//cout<<"\n ("<<i<<","<<j<<") Cell already in the temp arr.";
					}
				}
				else
				{
					//cout<<"\n ("<<i<<","<<j<<") not in perimeter.";
				}
			}
		}
	}
	
	if(temppc!=pc)
	{
		cout<<"\nError in sort_perimeter: The temppc: "<<temppc<<" and pc: "<<pc<<" dont match.";
		closegraph();
		while(1);
	}
	//copy the array back to perimeter array
	
	for(i=0;i<pc;i++)
	{
		perimeter[i*2] = temp_perimeter[i*2];
		perimeter[i*2+1] = temp_perimeter[i*2+1];
	}
	
}


void color_cells(int perimeter[], int prevperimetercolor1[], int prevperimetercolor2[], int pc, int cellstate[], int nbcells[], int nbcellcount)
{
	if(disp_board_en == 0){return;}
	if(showanim==0)
	{
		return;
	}
	int i=0, r=0, c=0, rloc=0, cloc=0;
	for(i=0;i<pc;i++)
	{
		
		r=perimeter[i*2];
		c=perimeter[i*2+1];
		
		rloc = cm*c+boardtx + 1;
		cloc = cm*r+boardty + 1;
		
		
		if(in_array2D(r, c, nbcells, nbcellcount, 2)>=0)
		{
			setfillstyle(nbstyle, nbcolor);
			floodfill(rloc, cloc, 0);
			continue;
		}
		
		if(page == 0)
		{
			if(prevperimetercolor1[i]==cellstate[i])
			{
				continue;
			}
			prevperimetercolor1[i] = cellstate[i];
		}
		else
		{
			if(prevperimetercolor2[i]==cellstate[i])
			{
				continue;
			}
			prevperimetercolor2[i] = cellstate[i];
		}
		
		switch(cellstate[i])
		{
			case 0:
				setfillstyle(unpstyle, unpcolor);
				break;

			case 1:
				setfillstyle(ppstyle, ppcolor);
				break;

			case 2:
				setfillstyle(fpstyle, fpcolor);
				break;
				
		}
		floodfill(rloc, cloc, 0);
	}
}


void chunk_solve()	//method 3
{
	
	int perimeter[length*width*2] = {0};
	int pc=0;
	int i=0,j=0,k=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(userarr[i*width+j]>0)
			{
				get_neightboring_covered_cells(i, j, perimeter, pc);
			}
		}
	}
	if(print_cell_probability_and_more_info){cout<<"\nCovered cell count in perimeter: "<<pc<<endl;}
	
	//We sort the perimeter to get the neighboring cells together
	sort_perimeter(perimeter, pc);
	
	int *prevperimetercolor1 = new int[pc];
	int *prevperimetercolor2 = new int[pc]; 
	if(showanim==1) //Animation optimisation
	{
		for(i=0;i<pc;i++)
		{
			prevperimetercolor1[i] = -1;
			prevperimetercolor2[i] = -1;
		}
		i=0;
	}
	
	
	sort_perimeter(perimeter, pc);
	
	//We start the new algorithm
	int tablesize = 4096, tablerowsize = pc+1;
	int *table1 = new int[tablerowsize*tablesize] , table1count=0;
	int *table2 = new int[tablerowsize*tablesize] , table2count=0;
	int cellstate[pc] = {0}; //0 for pending, 1 for calulated at least once, 2 for completed calculation
	
	
	int nbcells[20*2]; //Max number of points can be 15 at a time
	int nbcellcount = 0;
	int r, c;
	
	reset_table(table1, table1count, tablerowsize, tablesize);
	reset_table(table2, table2count, tablerowsize, tablesize);
	
	int n_cells_processed = number_of_cells_processed(2, cellstate, pc);
	
	
	while(n_cells_processed<pc)
	{
		delay(delay_between_anim);
		
		select_next_cell(r, c, cellstate, perimeter, pc);
		
		get_nbcells(r, c, nbcells, nbcellcount);
		
		prepdisplay(1);
		displayboard(1, (float)n_cells_processed/pc);
		
		color_cells(perimeter, prevperimetercolor1, prevperimetercolor2, pc, cellstate, nbcells, nbcellcount);
		
		display(1);
		
		change_cell_state(1, nbcells, nbcellcount, cellstate, perimeter, pc);
		
		fit(perimeter, pc, nbcells, nbcellcount, table1, table1count, table2, table2count, tablerowsize, tablesize);
		
		print_table(2, table2, table2count, tablerowsize, tablesize);
		
		shift_table2_to_table1(table1, table1count, table2, table2count, tablerowsize, tablesize);
		
		print_1darray(cellstate, pc);
		
		print_table(1, table1, table1count, tablerowsize, tablesize);
		
		shrink_table1(cellstate, perimeter, pc, table1, table1count, table2, table2count, tablerowsize, tablesize);
		
		print_table(1, table1, table1count, tablerowsize, tablesize);
		
		nbcellcount = 0;
		
		n_cells_processed=number_of_cells_processed(2, cellstate, pc);
	}
	
	prepdisplay(1);
	displayboard(1, 1);
	color_cells(perimeter, prevperimetercolor1, prevperimetercolor2, pc, cellstate, nbcells, nbcellcount);
	display(1);
	
	print_table(1, table1, table1count, tablerowsize, tablesize);
	
	float parr[pc] = {0};
	int valid_comb = table1[tablerowsize-1];
	
	if(print_cell_probability_and_more_info){cout<<"\nThe number of valid combinations are: "<<valid_comb;}
	
	if(print_cell_probability_and_more_info){cout<<"\n\nMine probability: ";}
	for(i=0;i<pc;i++)
	{
		mark_to_update(perimeter[i*2], perimeter[i*2+1]);
		parr[i] = (float)table1[i]/(float)valid_comb;
		if(print_cell_probability_and_more_info){cout<<"("<<perimeter[i*2]<<","<<perimeter[i*2+1]<<" = "<<parr[i]<<") ";}
	}
	
	delete [] table1;
	delete [] table2;
	delete [] prevperimetercolor1;
	delete [] prevperimetercolor2;
	
	make_decision(parr, perimeter, pc);
	
}

void microdisplay()
{
	cleardevice();
	int i=0,j=0;
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(userarr[i*width+j]==-2)
			{
				putpixel(i, j, WHITE);
			}
		}
	}
}

void ai_solve()
{
	switch(ai_method)
	{
		case 1:
			arb_prob_solve();
			break;
			
		case 2:
			perimeter_solve();
			break;
		
		default:
			chunk_solve();
			break;
	}
}

void benchmark(int ngames)
{
	int i=0, j=0, k=length*width, val=0, wins = 0, nmoves = 0, tmoves = 0;
	initdisplay();
	mingw_gettimeofday(&timebench1, NULL);
	
	for(val=0;val<ngames;val++)
	{
		cout<<"\nPlaying board number: "<<val+1;
		resetarrays();
		nmoves=0;
		isfirstmove = 1;
		prepdisplay();
		update_user_board();
		displayboard();
		display();
		while(digc<k-nbombs)
		{
			prepdisplay();
			update_user_board();
			displayboard();
			display();
			ai_solve();
			nmoves++;
			tmoves++;
			if(safe == 0)
			{
				break;
			}
		}
		if(safe)
		{
			if(print_cell_probability_and_more_info){cout<<"\n\nYOU WON!\n\n";}
			wins++;
		}
		else
		{
			if(print_cell_probability_and_more_info){cout<<"\n\nYOU LOSE!\n\n";}
		}
		if(disp_board_en==1)
		{
			prepdisplay();
			cleardevice();
			createbutton((char*)"Exit", max(min(boardtx+width*cm, screenx)-50, 0), min(boardty+length*cm+3, screeny-30), 0, 0);
			display();
		}
		if(disp_board_en==1)
		{
			prepdisplay();
			cleardevice();
			createbutton((char*)"Exit", max(min(boardtx+width*cm, screenx)-50, 0), min(boardty+length*cm+3, screeny-30), 0, 0);
			display();
		}
		
	}
		
	mingw_gettimeofday(&timebench2, NULL);
	
	cout<<"\n\nThe probability of winning is: "<<(float)wins/ngames;
	cout<<"\n\nThe average number of passes taken to solve a board is: "<<(float)tmoves/ngames;
	cout<<"\n\nThe time taken to solve is (in seconds): "<<(timebench2.tv_sec+0.000001*timebench2.tv_usec)-(timebench1.tv_sec + 0.000001*timebench1.tv_usec);
	cout<<"\n\nThe average time taken to solve each board is (in seconds): "<<((timebench2.tv_sec+0.000001*timebench2.tv_usec)-(timebench1.tv_sec + 0.000001*timebench1.tv_usec))/ngames;
	
}


int dist(int x1, int y1, int x2, int y2)
{
	return pow(pow(x1-x2,2)+pow(y1-y2,2),0.5);
}


int process_button_press()
{
	int i=0, index=-1;
	for(i=0;i<buttoncount;i++)
	{
		if(check_button(i,cpos.x,cpos.y))
		{
			index = i;
		}
	}
	
	if(print_cell_probability_and_more_info){cout<<"\nButton: "<<index<<endl;}
	switch(index)
	{
		case 0:
			closegraph();
			exit(0);
			break;
		case 1:
			ai_solve();
			break;
		case 2:
			cout<<"\nNumber of mines left are: "<<nbombs-nflags;
			break;
			
	}
	
	return index;
}


void multi_click(int r, int c)
{
	int i, j, mc=0, ucc=0;
	if(userarr[r*width+c]==-2)
	{
		dig(r, c);
		cout<<"is opened.";
		return;
	}
	
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(i==r && j==c)
			{
				continue;
			}
			if(userarr[i*width+j]==-1)
			{
				mc++;
			}
			if(userarr[i*width+j]>=0)
			{
				ucc++;
			}
		}
	}
	if(ucc+mc==8)
	{
		cout<<"but all of its neighboring cells are either flagged or dug!";
		return;
	}
	if(mc!=userarr[r*width+c])
	{
		cout<<"is not safe to open!";
		return;
	}
	cout<<"opened ";
	for(i = max(0, r-1); i < min(length-1, r+1)+1; i++)
	{
		for(j = max(0, c-1); j < min(width-1, c+1)+1; j++)
		{
			if(i==r && j==c)
			{
				continue;
			}
			else
			{
				if(userarr[i*width+j]==-2)
				{
					cout<<"("<<i<<","<<j<<") ";
					dig(i, j);
				}
			}
		}
	}

}

void select_cell(int bn)
{
	int i=boardtx, r=-1, c=-1;
	while(i<=cpos.x)
	{
		i += cm;
		c++;
	}
	i=boardty;
	while(i<=cpos.y)
	{
		i+=cm;
		r++;
	}

	cout<<"\nSelected cell ("<<r<<","<<c<<") ";
	
	
	if(isfirstmove==1)
	{
		isfirstmove = 0;
		createfixedmines(r, c);
		assignvalues();
		reset_probabilityarr();
	}
	
	if(bn==0)
	{
		if(flagarr[r*width+c]==1)
		{
			cout<<"is flagged! Unflag the cell first to dig it.";
		}
		else
		{
			
			multi_click(r, c);
		}
	}
	else
	{
		if(flagarr[r*width+c]==1)
		{
			cout<<"is unflagged.";
			unflagcell(r, c);
		}
		else
		{
			if(userarr[r*width+c]>=0)
			{
				cout<<"is dug no need to flag.";
			}
			else
			{
				cout<<"is flagged.";
				flagcell(r, c);
			}
		}
	}
	
	cout<<endl;
	
	
}


void first_move(int b1, int b2)
{
	int val, bn;
	prepdisplay();
	update_user_board();
	displayboard();
	display();
	bn = get_button_press();
	val = process_button_press();
	if(val<0 && cpos.x>=boardtx && cpos.y>=boardty && cpos.x<=b1 && cpos.y<=b2)
	{
		select_cell(bn);
	}
	
	
	
}

int main()
{
	int i=0, j=0;
	i = get_parameters();
	initialise_arrays();
	resetarrays();
	
	if(i>0)
	{
		benchmark(i);
		if(disp_board_en == 1){closegraph();}
		cout<<"\n\nEnter any value to exit: ";
		cin>>i;
		return 0;
	}
	int k = length*width, val=0, bn=0;
	int b1 = boardtx+width*cm, b2 = boardty+length*cm;
	initdisplay();

	if(start_seed<=0)
	{
		first_move(b1, b2);
	}
	else
	{
		isfirstmove = 0;
		createfixedmines();
		assignvalues();
		reset_probabilityarr();
	}
	
	mingw_gettimeofday(&timebench1, NULL);
	prepdisplay();
	update_user_board();
	displayboard();
	display();
	
	while(digc<k-nbombs)
	{
		prepdisplay();
		update_user_board();
		displayboard();
		display();
		
		bn = get_button_press();
		val = process_button_press();
		
		if(val<0 && cpos.x>=boardtx && cpos.y>=boardty && cpos.x<=b1 && cpos.y<=b2)
		{
			select_cell(bn);
		}
			
		if(safe == 0)
		{
			break;
		}
	
	}
	
	mingw_gettimeofday(&timebench2, NULL);
	const char* c2 = to_string((timebench2.tv_sec+0.000001*timebench2.tv_usec)-(timebench1.tv_sec + 0.000001*timebench1.tv_usec)).c_str();
	char c1[32+strlen(c2)] = "Time taken to solve in seconds: ";
	strcat(c1, c2);
	
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			if(mainarr[i*width+j]!=-1)
			{
				if(userarr[i*width+j]==-1)
				{
					userarr[i*width+j] = -2;
				}
				else
				{
					userarr[i*width+j] = mainarr[i*width+j];
				}
			}
			else
			{
				userarr[i*width+j] = mainarr[i*width+j];
			}
		}
	}
	
	prepdisplay();
	showanim = 1;
	displayboard(1,1);
	
	setcolor(LIGHTGREEN);
	settextstyle(buttonfont, 0, buttonfontsize);
	if(safe)
	{
		outtextxy(0, min(boardty+cm*length,screeny-textheight(c1)*2), (char*)"You Won! Press any key to exit.");
		cout<<"\n\nYOU WON!\n\n";
	}
	else
	{
		outtextxy(0, min(boardty+cm*length,screeny-textheight(c1)*2), (char*)"You Lose! Press any key to exit.");
		cout<<"\n\nYOU LOSE!\n\n";
	}
	
	outtextxy(0, min(boardty+cm*length+30,screeny-textheight(c1)), c1);
	
	display();
	getch();
	closegraph();
}
