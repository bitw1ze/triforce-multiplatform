#include <cstring>
#include <iostream>
#include <ctime>
#include <cstdlib>

#define NCOL 10
#define NROW 10

#ifdef WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

static const char *blockTypes = "!X$+=@";
int grid[NCOL][NROW];
int bottom = NROW-1;

using namespace std;

// Generate the blocks in a row to be pushed up
void generateRow () {
  bottom = (bottom + NROW - 1) % NROW;
  int ntypes = strlen(blockTypes);
  for (int i=0; i<NCOL; ++i) 
    grid[bottom][i] = rand() % ntypes;
}

// Print an individual row
void printRow(int row[]) {
  for (int i=0; i<NCOL; ++i) 
    cout.put (row[i] == -1 ? ' ' : blockTypes [row [i]]);
  cout.put('\n');
}

// Print the entire grid in a (messy) circular-queue fashion
void printGrid() {
    for ( int i=( (bottom + NROW - 1) % NROW ), j=0
        ; j < NROW
        ; i = (i + NROW - 1) % NROW, ++j ) 
      printRow (grid[i]);
}

int main(void) {
  srand(time(NULL));

  for (int i=0; i<NROW; ++i)
    for (int j=0; j<NCOL; ++j)
      grid[i][j] = -1;

  for (int i=NROW/2; i<NROW; ++i) 
    generateRow ();

  system(CLEAR);
  while (1) {
    printGrid();
    sleep(1);
    system(CLEAR);

    generateRow();
  }
}
