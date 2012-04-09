#include <cstring>
#include <iostream>
#include <ctime>
#include <cstdlib>

#define NCOL 10
#define NROW 10

static const char *blockTypes = "!X$+=@";
int grid[NCOL][NROW] = {0};
int bottom = NROW-1;

using namespace std;

void generateRow () {
  bottom = (bottom + NROW - 1) % NROW;
  int ntypes = strlen(blockTypes);
  for (int i=0; i<NCOL; ++i) {
    grid[bottom][i] = rand() % ntypes;
  }
}

void printRow(int row[]) {
  for (int i=0; i<NCOL; ++i) {
    cout.put (row[i] == 0 ? ' ' : blockTypes [row [i]]);
  }
  cout.put('\n');
}

void printGrid() {
    for (int i=( (bottom + NROW - 1) % NROW ), j=0; 
         j < NROW; 
         i = (i + NROW - 1) % NROW, ++j) {
      printRow (grid[i]);
    }
}

int main(void) {
  srand(time(NULL));

  for (int i=NROW/2; i<NROW; ++i) {
    generateRow ();
  }

  system("clear");
  while (1) {
    printGrid();
    sleep(1);
    system("clear");
    generateRow();
  }
}
