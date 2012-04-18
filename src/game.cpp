#include "game.h"

// Constants
const string GameEnv::themeDirectory = "themes\\classic\\";
const string GameEnv::blockFiles[] = {
	"block-blue.bmp",
	"block-green.bmp",
	"block-purple.bmp",
	"block-red.bmp",
	"block-special.bmp",
	"block-teal.bmp",
	"block-yellow.bmp"};
const string GameEnv::bgFile = "bg.bmp";
const string GameEnv::Menu::bgFile = "bg.bmp";

/* GameEnv methods */

/* Menu methods
   They are no longer inline. If there is a performance problem we can put it
   back to the way it was. Inline does come with problems. The project is
   messier with inline functions and all files that include a header with
   inline functions must be recompiled as well (not really a big deal for small
   projects, but it will make large projects more annoying to compile. */

GameEnv::Menu::Menu() { 
	current_frame = 0; 
	loadImages(); 
}

void GameEnv::Menu::composeFrame()
{
	if(Timer->elapsed(last_time,300))
	{
		//processFrame();
		last_time=Timer->time();
	}
	glutPostRedisplay();
}

void GameEnv::display() {
	if (showMenu)
		menu.display();
	else
		displayGame();
}

void GameEnv::Menu::display()
{
	composeFrame();
	background.drawGLbackground ();

	glutSwapBuffers();
}

void GameEnv::Menu::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  //  int r=254, g=0, b=254, frameCount=1, frame=0;
}

/* GameEnv methods */

GameEnv::GameEnv() { 
	showMenu = false;
	current_frame = 0; 
	loadImages(); 

	menu.Timer = Timer = new CTimer(); 
	Timer->start();
	last_time=Timer->time();
	last_pushtime = Timer->time();

	srand(time(NULL));

	grid = new Grid(this);
}

void GameEnv::displayGame() {
	composeFrame();
	background.drawGLbackground ();

	grid->display();

	glutSwapBuffers();
}

void GameEnv::processFrame()
{
	grid->setCoords();
}

void GameEnv::composeFrame()
{
	if (Timer->elapsed(last_pushtime, 900)) {
		grid->pushRow();
		last_pushtime = Timer->time();
	}

	if(Timer->elapsed(last_time,300))
	{
		processFrame();
		last_time=Timer->time();
		if(++current_frame>=1)
			current_frame=0;
	}
	glutPostRedisplay();
}


void GameEnv::loadImages()
{
  background.load( themeDirectory + bgFile );
  background.loadGLTextures();

  // r,g,b is background color to be filtered, frameCount and frame number
  int r=254, g=0, b=254, frameCount=1, frame=0;
  
  for (int i=0; i<nblocktypes; ++i) {
	  blockSprites[i] = new CBaseSprite(frameCount, background.getViewportWidth(),
		                                background.getViewportHeight());
	  blockSprites[i]->loadFrame(frame, themeDirectory + blockFiles[i],
	                             r, g, b);
	  blockSprites[i]->loadGLTextures();
  }
}

/* Grid methods */
Grid::Grid(GameEnv *ge) {
	blockSprites = ge->blockSprites;
	row_bottom = 0;
	row_top = 1;
	row_xvel = 0;
	row_yvel = 1;
	block_w = blockSprites[0]->GetWidth();
	block_h = blockSprites[0]->GetHeight();
	grid_x = ge->getWidth()/2 - (block_w * ncols)/2;
	grid_y = ge->getHeight() - block_h * 2;
	
	Timer = ge->Timer;

	for (int row=0; row<nrows/2; ++row) {
		pushRow();
	}
}

void Grid::pushRow() {
	if (blocks.size() == nrows)
		blocks.pop_back();

	Block *blockRow = new Block[ncols];

	for (int col=0; col<ncols; ++col) {
		blockRow[col].create(grid_x + col * block_w, - block_h * 2,
							 row_xvel, row_yvel, 
							 blockSprites[ rand() % nblocktypes ], 
							 Timer);
		printf("(%d, %d) ", blockRow[col].getX(), blockRow[col].getY());
	}
	cout << endl;
	row_bottom = (row_bottom + 1) % nrows;

	blocks.push_front(blockRow);
}

void Grid::display() {
	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled)
				(*it)[j].draw(0);
}

void Grid::setCoords() {
	int i = 0;
	for (deque<Block *>::iterator it = blocks.begin(); it < blocks.end(); ++it, ++i)
		for (int j=0; j<ncols; ++j) 
			if ((*it)[j].enabled) 
				(*it)[j].setY(grid_y - i * block_h);
}
