// Block Game
// main.cpp
// Game Revision 1.1
//
// 1.1 changes
// -Fixing a bug that causes the game to crash when pressing the space bar on the start menu.
// -Adding a toggle key to toggle the ghost block (none, partial, full).
// -Updated graphics for the ghost block.
// -A new game can no longer be started unless the game is paused.
// -Fixed a bug stopping the game from quitting when pressing the esc key when the game is paused.


// excluded unneeded header files
#define WIN32_LEAN_AND_MEAN
// include windows header
#include <Windows.h>
// for srand and rand
#include <stdlib.h>
// our bitmap object header
#include "bitmapobject.h"

// Give the window a name
#define WINDOWCLASS L"Blocks Game"

// Give the window a caption
#define WINDOWTITLE L"The Blocks Game"




// Using square tiles to define the window
// Tile size
const int TILESIZE = 16;
// Now the play area and sidebar
const int MAPWIDTH = 10;
const int MAPHEIGHT = 30;
const int GREY = 8;


// constants to define tile colours and no draw

const int TILENODRAW=-1;
const int TILEBLACK=0;
const int TILEGREY=1;
const int TILEBLUE=2;
const int TILERED=3;
const int TILEGREEN=4;
const int TILEYELLOW=5;
const int TILEWHITE=6;
const int TILESTEEL=7;
const int TILEPURPLE=8;

const int TILEZERO=9;
const int TILEONE=10;
const int TILETWO=11;
const int TILETHREE=12;
const int TILEFOUR=13;
const int TILEFIVE=14;
const int TILESIX=15;
const int TILESEVEN=16;
const int TILEEIGHT=17;
const int TILENINE=18;

const int TILEGREYUL = 19;
const int TILEGREYU = 20;
const int TILEGREYUR = 21;
const int TILEGREYR = 22;
const int TILEGREYBR = 23;
const int TILEGREYB = 24;
const int TILEGREYBL = 25;
const int TILEGREYL = 26;

const int TILEFADE = 27;
const int TILEPAUSE = 28; // Through to 33
const int TILEMATCH = 34; // Through to 41

const int TILESCORE = 42; // Through to 45
const int TILENEXT = 46; // Through to 49
const int TILELEVEL = 50; // Through to 53
const int TILEGHOST = 54;


// Declare functions

bool GameInit(); // Game initialization function
void GameLoop(); // Where the game takes place
void GameDone(); // Cleanup!
void DrawTile(int x, int y, int tile); // Coordinates and tile type
void DrawMap(); // Draw the whole map (basically render function)
void DrawHelp(); // Draw a help screen over the map
void NewBlock(); // Create a new block
void RotateBlock(); // Rotate a block if there's room
void Move(int x, int y); // Coordinates to move
int CollisionTest(int nx, int ny); // Test collision of blocks
void RemoveRow(int row); // Remove a row
void NewGame(); // Start a new game!

// Global Variables
HINSTANCE hInstMain = NULL; // Main app handle
HWND hWndMain = NULL; // Main window handle
bool bPause = true; // Pause the game with p
bool super_block = false; // Block that clears all of 1 colour if it's matched
int ghostBlock = 0;


// Array for the play area, extra row to make vertical movement collision detection easier

int Map[MAPWIDTH][MAPHEIGHT+1]; // The play area
int score = 0; // Variable to keep track of the score
int level = 1; // Variable to keep track of the level

//Sturcture for the play piece

struct Piece {
	int size[4][4];
	int x;
	int y;
};

// Piece variables for current piece and preview pane
// s to indicate a struct variable

Piece sPrePiece; // Preview pane piece
Piece sPiece; // Current piece

// Variables for timing

DWORD start_time; // Used in timing
bool GAMESTARTED = false; // Used by NewBlock()

// Play area for the game
BitMapObject bmoMap;
// Block images
BitMapObject bmoBlocks;

LRESULT CALLBACK TheWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Which message did we get?
	switch(uMsg)
	{
	case WM_CHAR:
		{
			
			if(bPause)
			{
				if(wParam == 0x050 || wParam == 0x070) // If P or p is pressed when paused
				{
					bPause = false; // Unpause the game
					return(0); // Handled Message
				}
				if(wParam == 0x04E || wParam == 0x06E) // If N or n is pressed then start a new game
				{
					NewGame();
					bPause = false;
					DrawMap();
					return(0);
				}
				// Else Do nothing
			}
			else
			{
				if(wParam == 0x050 || wParam == 0x070) // If P or p is pressed when unpaused
				{
					bPause = true; // Pause the game
					return(0); // Handled message
				}
				if(wParam == 0x047 || wParam == 0x067)// If G or g is pressed when unpaused
				{
					ghostBlock++;
					if(ghostBlock > 2)
					{
						ghostBlock =0;
					}
					DrawMap();
					return(0);
				}
			}
		}break;
	case WM_KEYDOWN: // Key was pressed
		{
			//if(!GAMESTARTED)
			//{
			//	NewGame();
			//	return(0);
			//}
			if(wParam == VK_ESCAPE) // Check for ESC key
			{
				DestroyWindow(hWndMain);
				return(0); // Handled message
			}
			if(bPause)
			{
				return(0);
			}
			if(wParam == VK_DOWN) // Check for down arrow
			{
				Move(0,1);
				return(0); // Handled message
			}
			if(wParam == VK_UP) // Check for up arrow
			{
				RotateBlock();
				return (0); // Handled message
			}
			if(wParam == VK_LEFT) // Check for left arrow
			{
				Move(-1,0);
				return(0); // Handled message
			}
			if(wParam == VK_RIGHT) // Check for right arrow
			{
				Move(1,0);
				return(0); // Handled message
			}
			if(wParam == VK_SPACE) // Check for spacebar
			{
				while(!CollisionTest(0, 1))
					Move(0, 1);
				Move(0, 1);
				return(0); // Handled message
			}

		}break;
	case WM_DESTROY: // Window is being destroyed
		{
			// Tell the application we are quitting
			PostQuitMessage(0);
			return(0); // Handled message
		}break;
	case WM_PAINT: // Window needs redrawing
		{
			// A variable needed for painting information
			PAINTSTRUCT ps;

			// Start painting
			HDC hdc = BeginPaint(hwnd, &ps);

			// Redraw the map
			BitBlt(hdc, 0, 0, TILESIZE * MAPWIDTH + TILESIZE * GREY, TILESIZE * MAPHEIGHT, bmoMap, 0 , 0, SRCCOPY);

			// End painting
			EndPaint(hwnd, &ps);

			return(0); // Handled message
		}break;
	}

	// Pass along any other message to the default message handler
	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Assign instance to global variable
	hInstMain = hInstance;

	// Create window class
	WNDCLASSEX wcx;

	// Set the size of the structure
	wcx.cbSize = sizeof(WNDCLASSEX);

	// Class style
	wcx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	//window procedure
	wcx.lpfnWndProc = TheWindowProc;

	// Class extra
	wcx.cbClsExtra = 0;

	//Window extra
	wcx.cbWndExtra = 0;

	// Application handle
	wcx.hInstance = hInstMain;

	// Icon
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	// Cursor
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);

	// Background colour
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	// Menu
	wcx.lpszMenuName = NULL;

	// Class name
	wcx.lpszClassName = WINDOWCLASS;

	// Small icon
	wcx.hIconSm = NULL;

	// Register the window class, return 0 on fail
	if(!RegisterClassEx(&wcx))
		return(0);

	// Create main window
	hWndMain = CreateWindowEx(0, WINDOWCLASS, WINDOWTITLE, WS_BORDER | WS_SYSMENU | WS_CAPTION | WS_VISIBLE, 0, 0, 320, 240, NULL, NULL, hInstMain, NULL);
	
	// Error check
	if(!hWndMain)
		return(0);

	// If program initialization failed then return 0
	if(!GameInit())
		return(0);

	// Message structure
	MSG msg;

	// Message pump
	for( ; ; )
	{
		// Look for a message
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// There is a message
			// Check that we aren't quitting
			if(msg.message == WM_QUIT)
				break;

			// Translate the message
			TranslateMessage(&msg);

			// Dispatch the message
			DispatchMessage(&msg);
		}

		// Run main game loop
		GameLoop();
	}

	// Clean up the program data
	GameDone();

	// Return the wparam from the WM_QUIT message
	return(msg.wParam);
}

bool GameInit()
{
	// Set the client area size
	RECT rcTemp;
	SetRect(&rcTemp, 0 ,0, MAPWIDTH * TILESIZE + TILESIZE * GREY, MAPHEIGHT * TILESIZE); // 160*480 client area
	AdjustWindowRect(&rcTemp, WS_BORDER | WS_SYSMENU | WS_CAPTION | WS_VISIBLE, FALSE); // Adjust the window size based on the desired client area
	SetWindowPos(hWndMain, NULL, 0 , 0, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, SWP_NOMOVE); // Set the window width and height

	// Create the play area image
	HDC hdc = GetDC(hWndMain);
	bmoMap.Create(hdc, MAPWIDTH * TILESIZE + TILESIZE * GREY, MAPHEIGHT * TILESIZE);
	FillRect(bmoMap, &rcTemp, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ReleaseDC(hWndMain, hdc);

	bmoBlocks.Load(NULL, L"blocks.bmp");
	
	DrawMap();
	// NewGame();

	return(true); // Return success
}

void GameDone()
{
	// Clean up code goes here
}

void GameLoop()
{
	if(!GAMESTARTED)
		return;
	if(bPause) // If paused, do nothing
	{
		DrawMap();
		return;
	}
	if( (GetTickCount() - start_time) > (2000/(level + 1)))
	{
		Move(0,1);
		start_time = GetTickCount();
	}
}

void NewGame()
{
	start_time = GetTickCount();
	GAMESTARTED = false;
	score = 0;
	level = 1;

	// Start out the map
	for(int x=0; x < MAPWIDTH; x++)
	{
		for(int y=0; y < MAPHEIGHT; y++)
		{
				Map[x][y] = TILEBLACK;
		}
	}
	NewBlock();

	DrawMap();
}

void DrawTile(int x, int y, int tile) // Paint a tile
{
	// Mask first
	BitBlt(bmoMap, x * TILESIZE, y * TILESIZE, TILESIZE, TILESIZE, bmoBlocks, tile * TILESIZE, TILESIZE, SRCAND);
	// Then image
	BitBlt(bmoMap, x * TILESIZE, y * TILESIZE, TILESIZE, TILESIZE, bmoBlocks, tile * TILESIZE, 0, SRCPAINT);
}

void DrawHelp() // Draws the help
{
	int x, y;
	int temp, temp2;
	int tile = 0;

	for(x = 2; x < 16; x++)
	{
		for(y = 3; y < MAPHEIGHT-3; y++)
		{
			if(x == 2)
			{
				if(y == 3)
				{
					DrawTile(x, y, TILEGREYUL);
				}
				else
				{
					if(y == MAPHEIGHT -4)
					{
						DrawTile(x, y, TILEGREYBL);
					}
					else
					{
						DrawTile(x, y, TILEGREYL);
					}
				}
			}
			else
			{
				if(x == 15)
				{
					if(y == 3)
					{
						DrawTile(x, y, TILEGREYUR);
					}
					else
					{
						if(y == MAPHEIGHT -4)
						{
							DrawTile(x, y, TILEGREYBR);
						}
						else
						{
							DrawTile(x, y, TILEGREYR);
						}
					}
				}
				else
				{
					if(y == 3)
					{
						DrawTile(x, y, TILEGREYU);
					}
					else
					{
						if(y == MAPHEIGHT -4)
						{
							DrawTile(x, y, TILEGREYB);
						}
						else
						{
							DrawTile(x, y, TILEGREY);
						}
					}
				}
			}
		}
	}
	for(x = 0; x < 14; x++)
	{
		temp = 0;
		temp2 = 0;
		for(y = 0; y < 24; y++)
		{
			if(y > 7)
			{
				temp = 14;
				temp2 = 8;
			}
			if(y > 15)
			{
				temp = 28;
				temp2 = 16;
			}
			// Mask first
			BitBlt(bmoMap, (x+2) * TILESIZE, (y+3) * TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (x+temp)*TILESIZE, ((y-temp2)*2+3)*TILESIZE, SRCAND);
			// Then image
			BitBlt(bmoMap, (x+2) * TILESIZE, (y+3) * TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (x+temp)*TILESIZE, ((y-temp2)*2+2)*TILESIZE, SRCPAINT);
		}
	}

	temp = score;
	DrawTile((MAPWIDTH+GREY)/2 - 3, MAPHEIGHT-5,(9 + temp/100000));
	if(temp >= 100000)
		temp -= (100000 * (temp/100000));
	DrawTile((MAPWIDTH+GREY)/2 - 2, MAPHEIGHT-5, (9 + temp/10000));
	if(temp >= 10000)
		temp -= (10000 * (temp/10000));
	DrawTile((MAPWIDTH+GREY)/2 - 1, MAPHEIGHT-5, (9 + temp/1000));
	if(temp >= 1000)
		temp -= (1000 * (temp/1000));
	DrawTile((MAPWIDTH+GREY)/2, MAPHEIGHT-5, (9 + temp/100));
	if(temp >= 100)
		temp -= (100 * (temp/100));
	DrawTile((MAPWIDTH+GREY)/2 + 1, MAPHEIGHT-5, (9 + temp/10));
	if(temp >= 10)
		temp -= (10 * (temp/10));
	DrawTile((MAPWIDTH+GREY)/2 + 2, MAPHEIGHT-5, (9 + temp));
}

void DrawMap() // Draw the screen
{
	int xmy, ymx, temp;

	// Place the toolbar
	for(xmy = MAPWIDTH; xmy < MAPWIDTH + GREY; xmy++)
	{
		for(ymx = 0; ymx < MAPHEIGHT; ymx++)
		{
			if(xmy == MAPWIDTH)
			{
				if(ymx == 0)
				{
					DrawTile(xmy, ymx, TILEGREYUL);
				}
				else
				{
					if(ymx == MAPHEIGHT -1)
					{
						DrawTile(xmy, ymx, TILEGREYBL);
					}
					else
					{
						DrawTile(xmy, ymx, TILEGREYL);
					}
				}
			}
			else
			{
				if(xmy == MAPWIDTH + GREY - 1)
				{
					if(ymx == 0)
					{
						DrawTile(xmy, ymx, TILEGREYUR);
					}
					else
					{
						if(ymx == MAPHEIGHT -1)
						{
							DrawTile(xmy, ymx, TILEGREYBR);
						}
						else
						{
							DrawTile(xmy, ymx, TILEGREYR);
						}
					}
				}
				else
				{
					if(ymx == 0)
					{
						DrawTile(xmy, ymx, TILEGREYU);
					}
					else
					{
						if(ymx == MAPHEIGHT -1)
						{
							DrawTile(xmy, ymx, TILEGREYB);
						}
						else
						{
							DrawTile(xmy, ymx, TILEGREY);
						}
					}
				}
			}
		}
	}
	// Draw preview block
	for(temp = 0; temp < 4; temp++)
	{
		DrawTile(MAPWIDTH+2+temp, 1, TILENEXT + temp);
	}
	for(xmy = 0; xmy < 4; xmy++)
		for(ymx = 0; ymx < 4; ymx++)
			if(sPrePiece.size[xmy][ymx] != TILENODRAW)
				DrawTile(sPrePiece.x+xmy, sPrePiece.y+ymx+1, sPrePiece.size[xmy][ymx]);

	// Draw the score
	for(temp = 0; temp < 4; temp++)
	{
		DrawTile(MAPWIDTH+2+temp, 8, TILESCORE + temp);
	}

	temp = score;
	DrawTile(MAPWIDTH+1, 10, (9 + temp/100000));
	if(temp >= 100000)
		temp -= (100000 * (temp/100000));
	DrawTile(MAPWIDTH+2, 10, (9 + temp/10000));
	if(temp >= 10000)
		temp -= (10000 * (temp/10000));
	DrawTile(MAPWIDTH+3, 10, (9 + temp/1000));
	if(temp >= 1000)
		temp -= (1000 * (temp/1000));
	DrawTile(MAPWIDTH+4, 10, (9 + temp/100));
	if(temp >= 100)
		temp -= (100 * (temp/100));
	DrawTile(MAPWIDTH+5, 10, (9 + temp/10));
	if(temp >= 10)
		temp -= (10 * (temp/10));
	DrawTile(MAPWIDTH+6, 10, (9 + temp));

	//Draw the level
	for(temp = 0; temp < 4; temp++)
	{
		DrawTile(MAPWIDTH+2+temp, MAPHEIGHT - 5, TILELEVEL + temp);
	}
	temp = level;
	DrawTile(MAPWIDTH+3, MAPHEIGHT-3, (9 + temp/10));
	if(temp >= 10)
		temp -= (10 *(temp/10));
	DrawTile(MAPWIDTH+4, MAPHEIGHT-3, (9 + temp));

	// Draw the play area
	// Loop through the positions
	for(xmy = 0; xmy < MAPWIDTH; xmy++)
	{
		for(ymx = 0; ymx < MAPHEIGHT; ymx++)
		{
			DrawTile(xmy, ymx, Map[xmy][ymx]);
		}
	}


	// Draw the ghost block
	if(!bPause && GAMESTARTED)
	{	
		temp = 1;
		while(!CollisionTest(0,temp+1))
		{
			temp++;
		}
		if(temp > 4)
		{
			for(xmy = 0; xmy < 4; xmy++)
			{
				for(ymx = 0; ymx < 4; ymx++)
				{
					if(sPiece.size[xmy][ymx] != TILENODRAW)
					{	
						if(ghostBlock == 1) // Partial Ghost Block
						{	// Mask first
							BitBlt(bmoMap, (sPiece.x+xmy)*TILESIZE, (sPiece.y+ymx+temp)*TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (sPiece.size[xmy][ymx]+40)*TILESIZE, 3*TILESIZE, SRCAND);
							// Then image
							BitBlt(bmoMap, (sPiece.x+xmy)*TILESIZE, (sPiece.y+ymx+temp)*TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (sPiece.size[xmy][ymx]+40)*TILESIZE, 2*TILESIZE, SRCPAINT);
							// DrawTile(sPiece.x+xmy, sPiece.y+ymx+temp, TILEGHOST);
						}
						if(ghostBlock == 2) // Full Ghost Block
						{	// Mask first
							BitBlt(bmoMap, (sPiece.x+xmy)*TILESIZE, (sPiece.y+ymx+temp)*TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (sPiece.size[xmy][ymx]+40)*TILESIZE, 5*TILESIZE, SRCAND);
							// Then image
							BitBlt(bmoMap, (sPiece.x+xmy)*TILESIZE, (sPiece.y+ymx+temp)*TILESIZE, TILESIZE, TILESIZE, bmoBlocks, (sPiece.size[xmy][ymx]+40)*TILESIZE, 4*TILESIZE, SRCPAINT);
						}
					}
				}
			}
		}
	}
	// Draw the moving block
	for(xmy = 0; xmy < 4; xmy++)
	{
		for(ymx = 0; ymx < 4; ymx++)
		{
			if(sPiece.size[xmy][ymx] != TILENODRAW)
			{
				DrawTile(sPiece.x+xmy, sPiece.y+ymx, sPiece.size[xmy][ymx]);
			}
		}
	}

	// Fade if paused
	if(bPause || !GAMESTARTED)
	{
		temp = 0;
		for(xmy = 0; xmy < MAPWIDTH; xmy++)
		{
			for(ymx = 0; ymx < MAPHEIGHT; ymx++)
			{
				DrawTile(xmy, ymx, TILEFADE);
			}
		}
		//for(xmy = (MAPWIDTH/2)-3; xmy < (MAPWIDTH/2)+3; xmy++)
		//{
		//	ymx = MAPHEIGHT/2;
		//	DrawTile(xmy, ymx, TILEPAUSE + temp);
		//	temp++;
		//}
		DrawHelp();
	}

	// Invalidate the window rect
	InvalidateRect(hWndMain, NULL, FALSE);
}

void NewBlock()
{
	int newblock;
	int i, j;

	// Block Types
	//  0  1   2    3    4    5   6
	//   X
	//   X  XX   X   XX    XX  XX  XX
	//   X  XX  XXX   XX  XX    X  X
	//   X                      X  X

	// Begin game! Generate a block and then a preview block

	srand(GetTickCount());

	// Initialize the piece to all blank
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			sPiece.size[i][j] = TILENODRAW;

	sPiece.x = MAPWIDTH/2 - 2;
	sPiece.y = -1;

	// Check if the game has started
	if(GAMESTARTED == false)
	{
		//Generate the first piece
		GAMESTARTED = true;

		newblock = rand() % 7;

		switch(newblock)
		{
		case 0: // Tower
			{
				sPiece.size[1][0] = (rand() % 7 + 2);
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[1][3] = (rand() % 7 + 2);
				sPiece.y = 0;
			}break;
		case 1: // Box
			{
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[2][1] = (rand() % 7 + 2);
				sPiece.size[2][2] = (rand() % 7 + 2);
			}break;
		case 2: // Pyramid
			{
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[0][2] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[2][2] = (rand() % 7 + 2);
			}break;
		case 3: // Left Leaner
			{
				sPiece.size[0][1] = (rand() % 7 + 2);
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[2][2] = (rand() % 7 + 2);
			}break;
		case 4: // Right Leaver
			{
				sPiece.size[2][1] = (rand() % 7 + 2);
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[0][2] = (rand() % 7 + 2);
			}break;
		case 5: // Left Knight
			{
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[2][1] = (rand() % 7 + 2);
				sPiece.size[2][2] = (rand() % 7 + 2);
				sPiece.size[2][3] = (rand() % 7 + 2);
			}break;
		case 6: // Right Knight
			{
				sPiece.size[2][1] = (rand() % 7 + 2);
				sPiece.size[1][1] = (rand() % 7 + 2);
				sPiece.size[1][2] = (rand() % 7 + 2);
				sPiece.size[1][3] = (rand() % 7 + 2);
			}break;
		}
	}
	else
	{
		int temp = 0;
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				sPiece.size[i][j] = sPrePiece.size[i][j];
				if(sPiece.size[i][j] != TILENODRAW)
				{
					temp++;
				}
			}
		}
		if(temp == 1)
		{
			super_block = true;
		}

	}

	newblock = rand() % 7;

	for(i = 0; i < 4; i++)
		for(j = 0; j <4; j++)
			sPrePiece.size[i][j] = TILENODRAW;

	sPrePiece.x = MAPWIDTH + GREY / 4;
	sPrePiece.y = GREY / 4;

	if(!(rand() % 10))
	{
		sPrePiece.size[2][2] = (rand() % 7 + 2);
	}
	else
	{
		switch(newblock)
		{
			case 0: // Tower
			{
				sPrePiece.size[1][0] = (rand() % 7 + 2);
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[1][3] = (rand() % 7 + 2);
			}break;
			case 1: // Box
			{
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[2][1] = (rand() % 7 + 2);
				sPrePiece.size[2][2] = (rand() % 7 + 2);
			}break;
			case 2: // Pyramid
			{
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[0][2] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[2][2] = (rand() % 7 + 2);
			}break;
			case 3: // Left Leaner
			{
				sPrePiece.size[0][1] = (rand() % 7 + 2);
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[2][2] = (rand() % 7 + 2);
			}break;
			case 4: // Right Leaver
			{
				sPrePiece.size[2][1] = (rand() % 7 + 2);
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[0][2] = (rand() % 7 + 2);
			}break;
			case 5: // Left Knight
			{
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[2][1] = (rand() % 7 + 2);
				sPrePiece.size[2][2] = (rand() % 7 + 2);
				sPrePiece.size[2][3] = (rand() % 7 + 2);
			}break;
			case 6: // Right Knight
			{
				sPrePiece.size[2][1] = (rand() % 7 + 2);
				sPrePiece.size[1][1] = (rand() % 7 + 2);
				sPrePiece.size[1][2] = (rand() % 7 + 2);
				sPrePiece.size[1][3] = (rand() % 7 + 2);
			}break;
		}
	}
	DrawMap();
}

void RotateBlock()
{
	int i, j, temp[4][4], x, y;
	int tempPos = 0; // Variable to displace the rotated block back on the board for collision checks
	bool moveTile = false; // Indicates when a block is off the play area

	// Copy and Rotate to the temporary array
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			temp[3-j][i] = sPiece.size[i][j];
	
	// Check for collision with side
	x = sPiece.x;
	while(x < 0) // Checks for blocks off the left side and moves them right
	{
		for(y = sPiece.y; y < sPiece.y + 4; y++)
			if(temp[x - sPiece.x][y - sPiece.y] != TILENODRAW)
				moveTile = true;

		if(moveTile)
		{
			tempPos++;
			moveTile = false;
		}
		x++;
	}
	x = sPiece.x + 3;
	while(x >= MAPWIDTH) // Checks for blocks off the right side and moves them left
	{
		for(y = sPiece.y; y < sPiece.y + 4; y++)
			if(temp[x - sPiece.x][y - sPiece.y] != TILENODRAW)
				moveTile = true;
		
		if(moveTile)
		{
			tempPos--;
			moveTile = false;
		}
		x--;
	}
	
	// Check for collision with blocks
	for(x = 0; x < MAPWIDTH; x++)
		for(y = 0; y < MAPHEIGHT; y++)
			if(x >= sPiece.x + tempPos && x < sPiece.x + 4 + tempPos)
				if(y >= sPiece.y && y < sPiece.y + 4)
					if(Map[x][y] != TILEBLACK)
						if(temp[x - sPiece.x][y - sPiece.y] != TILENODRAW)
							return;
	// End of collision check

	// Successful! Copy the tempoary array to the original piece
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			sPiece.size[i][j] = temp[i][j];

	sPiece.x += tempPos;

	DrawMap();

	return;
}

void Move(int x, int y)
{
	if(CollisionTest(x, y))
	{
		if(y == 1)
		{
			if(sPiece.y < 1)
			{
				// Game over
				//NewGame();
				GAMESTARTED = false;
				bPause = true;
			}
			else
			{
				int scoreMultiplier = 0;
				
				int i, j;
				int matchLeft, matchRight, matchUp, matchDown;
				int temp, temp2, temp3;
				DWORD fade_timer;
				bool fading_block = false;

				// Time for a new block
				for(i = 0; i < 4; i++)
					for(j = 0; j < 4; j++)
						if(sPiece.size[i][j] != TILENODRAW)
							Map[sPiece.x+i][sPiece.y+j] = sPiece.size[i][j];
				
				if(super_block)
				{
					for(i = 0; i < 4; i++)
					{
						for(j = 0; j < 4; j++)
						{
							if(sPiece.size[i][j] != TILENODRAW)
							{
								if(Map[sPiece.x+i][sPiece.y+j+1] == sPiece.size[i][j])
								{
									for(temp = 0; temp < MAPHEIGHT; temp++)
									{
										for(temp2 = 0; temp2 < MAPWIDTH; temp2++)
										{
											if(Map[temp2][temp] == sPiece.size[i][j])
											{
												Map[temp2][temp] = TILEMATCH;
												fading_block = true;
											}
										}
									}
								}
							}
						}
					}
					if(fading_block == true)
					{
						DrawMap();
				
						// A variable needed for painting information
						PAINTSTRUCT ps;
						// Start painting
						HDC hdc = BeginPaint(hWndMain, &ps);
						// Redraw the map
						BitBlt(hdc, 0, 0, TILESIZE * MAPWIDTH + TILESIZE * GREY, TILESIZE * MAPHEIGHT, bmoMap, 0 , 0, SRCCOPY);
						// End painting
						EndPaint(hWndMain, &ps);
				
						fade_timer = GetTickCount();
						while(GetTickCount() - fade_timer < 100)
						{}
						fading_block = false;
					}
					super_block = false;
				}


				// Check for matched colours before calling the new block
				// Check each newly added block piece on the map horizontally
				// and vertially for sets of 5 or more and set those block to TILEMATCH
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						if(sPiece.size[i][j] != TILENODRAW)
						{
							matchLeft = 1;
							matchRight = 1;
							matchUp = 1;
							matchDown = 1;

							while(Map[sPiece.x+i-matchLeft][sPiece.y+j] == sPiece.size[i][j])
							{
								if(sPiece.x+i-matchLeft < 0)
									break;
								matchLeft++;
							}
							matchLeft--;
							
							while(Map[sPiece.x+i+matchRight][sPiece.y+j] == sPiece.size[i][j])
							{
								if(sPiece.x+i+matchRight >= MAPWIDTH)
									break;
								matchRight++;
							}
							matchRight--;

							while(Map[sPiece.x+i][sPiece.y+j-matchUp] == sPiece.size[i][j])
							{
								if(sPiece.y+j-matchUp < 0)
									break;
								matchUp++;
							}
							matchUp--;

							while(Map[sPiece.x+i][sPiece.y+j+matchDown] == sPiece.size[i][j])
							{
								if(sPiece.x+j-matchDown >= MAPHEIGHT)
									break;
								matchDown++;
							}
							matchDown--;

							if(matchLeft + matchRight + 1 > 4)
							{
								Map[sPiece.x+i][sPiece.y+j] += (matchLeft*10) + (matchRight*100);
							}
							if(matchUp + matchDown + 1 > 4)
							{
								Map[sPiece.x+i][sPiece.y+j] += (matchUp*1000) + (matchDown*10000);
							}
						}
					}
				}

				// Create the new block before checking for and removing blocks for a 5+ colour match
				NewBlock();

				// Mark blocks for removal if a 5+ colour match was made
				for(j = 0; j < MAPHEIGHT; j++)
				{
					for(i = 0; i < MAPWIDTH; i++)
					{
						temp = Map[i][j];

						if(temp != TILEMATCH)
						{
							if(temp > 9)
							{
								if(temp > 999)
								{
									temp2 = temp/10000;
									while(temp2 >= 0)
									{
										for(temp3 = 0; temp3 < MAPWIDTH; temp3++)
										{
											if(Map[temp3][j+temp2] < 10)
											{
												if(Map[temp3][j+temp2] != TILEBLACK)
													Map[temp3][j+temp2] = TILEMATCH;
											}
										}
										temp2--;
									}
									temp -= 10000 * (temp/10000);
									temp2 = temp/1000;
									while(temp2 > 0)
									{
										for(temp3 = 0; temp3 < MAPWIDTH; temp3++)
										{
											if(Map[temp3][j-temp2] < 10)
											{
												if(Map[temp3][j-temp2] != TILEBLACK)
													Map[temp3][j-temp2] = TILEMATCH;
											}
										}
										temp2--;
									}
									temp -= 1000 * (temp/1000);
								}
								if(temp > 9)
								{
									temp2 = temp/100;
									while(temp2 >= 0)
									{
										for(temp3 = 0; temp3 < MAPHEIGHT; temp3++)
										{
											if(Map[i+temp2][temp3] < 10)
											{
												if(Map[i+temp2][temp3] != TILEBLACK)
													Map[i+temp2][temp3] = TILEMATCH;
											}
										}
										temp2--;
									}
									temp -= 100 * (temp/100);
									temp2 = temp / 10;
									while(temp2 > 0)
									{
										for(temp3 = 0; temp3 < MAPHEIGHT; temp3++)
										{
											if(Map[i-temp2][temp3] < 10)
											{
												if(Map[i-temp2][temp3] != TILEBLACK)
														Map[i-temp2][temp3] = TILEMATCH;
											}
										}
										temp2--;
									}
								}
								Map[i][j] = TILEMATCH;
								fading_block = true;
							}
						}
					}

					if(fading_block == true)
					{
						DrawMap();
				
						// A variable needed for painting information
						PAINTSTRUCT ps;
						// Start painting
						HDC hdc = BeginPaint(hWndMain, &ps);
						// Redraw the map
						BitBlt(hdc, 0, 0, TILESIZE * MAPWIDTH + TILESIZE * GREY, TILESIZE * MAPHEIGHT, bmoMap, 0 , 0, SRCCOPY);
						// End painting
						EndPaint(hWndMain, &ps);
				
						fade_timer = GetTickCount();
						while(GetTickCount() - fade_timer < 100)
						{}
						fading_block = false;
					}
				}

				// Clear the marked blocks
				scoreMultiplier = 0;
				for(temp = TILEMATCH+1; temp < TILEMATCH + 8; temp++)
				{
					for(j = 0; j < MAPHEIGHT; j++)
					{
						for(i = 0; i < MAPWIDTH; i++)
						{
							if(Map[i][j] == temp - 1)
							{
								scoreMultiplier++;
								Map[i][j] = temp;
								fading_block = true;
							}
						}
					}

					if(fading_block == true)
					{
						DrawMap();
				
						// A variable needed for painting information
						PAINTSTRUCT ps;
						// Start painting
						HDC hdc = BeginPaint(hWndMain, &ps);
						// Redraw the map
						BitBlt(hdc, 0, 0, TILESIZE * MAPWIDTH + TILESIZE * GREY, TILESIZE * MAPHEIGHT, bmoMap, 0 , 0, SRCCOPY);
						// End painting
						EndPaint(hWndMain, &ps);
				
						fade_timer = GetTickCount();
						while(GetTickCount() - fade_timer < 100)
						{}
						fading_block = false;
					}
				}
				for(j = 0; j < MAPHEIGHT; j++)
				{	
					for(i = 0; i < MAPWIDTH; i++)
					{
						if(Map[i][j] == TILEMATCH + 7)
						{
							for(temp = j; temp > 0; temp--)
							{
								Map[i][temp] = Map[i][temp-1];
							}
						}
					}
				}

				scoreMultiplier = scoreMultiplier/7;
				while(scoreMultiplier > 0)
				{
					score += scoreMultiplier*level;
					scoreMultiplier--;
				}


				// Check for a cleared row
				for(j = 0; j < MAPHEIGHT; j++)
				{
					bool filled = true;
					for(i = 0; i < MAPWIDTH; i++)
						if(Map[i][j] == TILEBLACK)
							filled = false;

					if(filled)
					{
						RemoveRow(j);
						scoreMultiplier++;
					}
				}
				//NewBlock();

				if(scoreMultiplier)
					score += 10 * scoreMultiplier * scoreMultiplier * level;
				score += 1 * level;

				while(score >= 200 * level * level)
					level++;
			}
		}
	}
	else
	{
		sPiece.x+=x;
		sPiece.y+=y;
	}
	DrawMap();
}

int CollisionTest(int nx, int ny)
{
	int newx = sPiece.x+nx;
	int newy = sPiece.y+ny;

	int i, j, x, y;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(sPiece.size[i][j] != TILENODRAW)
				if(newx + i < 0 || newx + i > MAPWIDTH - 1 || /*newy + j < 0 ||*/ newy + j > MAPHEIGHT - 1)
					return 1;

	for(x = 0; x < MAPWIDTH; x++)
		for(y = 0; y < MAPHEIGHT; y++)
			if(x >= newx && x < newx + 4)
				if(y >= newy && y < newy + 4)
					if(Map[x][y] != TILEBLACK)
						if(sPiece.size[x - newx][y-newy] != TILENODRAW)
							return 1;

	return 0;
}

void RemoveRow(int row)
{
	int x, y;
	int counter = 0;
	DWORD fade_timer;

	for(counter = TILEMATCH; counter < TILEMATCH+8; counter++)
	{
		y = row;
		for(x = 0; x < MAPWIDTH; x++)
		{
			Map[x][y] = counter;
			DrawTile(x, y, counter);
		}

		DrawMap();

		// A variable needed for painting information
		PAINTSTRUCT ps;
		// Start painting
		HDC hdc = BeginPaint(hWndMain, &ps);
		// Redraw the map
		BitBlt(hdc, 0, 0, TILESIZE * MAPWIDTH + TILESIZE * GREY, TILESIZE * MAPHEIGHT, bmoMap, 0 , 0, SRCCOPY);
		// End painting
		EndPaint(hWndMain, &ps);

		fade_timer = GetTickCount();
		while(GetTickCount() - fade_timer < 100)
		{}
	}

	for(x = 0; x < MAPWIDTH; x++)
	{
		for(y = row; y > 0; y--)
		{
			Map[x][y] = Map[x][y-1];
		}
	}
}