#include "grid.h"
#include "game.h"

using namespace std;

Grid::Grid(int cols, int rows, vector<string> & blkFiles) {
	ncols = cols;
	nrows = rows;
	for (int i=0; i<blkFiles.size(); ++i)
		blockFiles.push_back(blkFiles[i]);
}

void Grid::generate(const GameEnv &gameEnv) {
	int xspeed=0, yspeed=1;
	for (int i=0; i<rows(); ++i) {
		for (int j=0; j<cols(); ++j) {
		  matrix[i][j].create(x+j*blockSprites[i][j]->GetWidth(), y-i*blockSprites[i][j]->GetHeight(), xspeed, yspeed, blockSprites[i][j], gameEnv.Timer);
		  matrix[i][j].create(
	  }
  }
}

bool Grid::LoadImages()
{
  int r=254, g=0, b=254, frameCount=1, frame=0;    // r,g,b is background color to be filtered, frameCount and frame number
  
  for (int i=0; i<rows(); ++i) {
	  for (int j=0; j<cols(); ++j) {
		  blockSprites[i][j] = new CBaseSprite(frameCount, background.getViewportWidth(), background.getViewportHeight());
		  blockSprites[i][j]->loadFrame(frame, blockFiles[rand() % blockFiles.size()], r, g, b);
		  blockSprites[i][j]->loadGLTextures();
	  }
  }

  return true;
}

