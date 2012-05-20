#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "optparse.h"

/*
 * Global Vars
 */
int height, width = 0;
char *path        = NULL;
_Bool automode    = false;
/*
 * My Structs
 */
typedef struct {
  _Bool alive;
  _Bool alive_next_round;
} cell_t;

typedef struct {
  cell_t **cells;
} world_t;

/*
 * Functions
 */
void allocateMemory(world_t *world);
int countNeighbours(int x, int y, world_t *world);
_Bool fillWorld(world_t *world);
_Bool create_world(world_t *world);
void printWorld(world_t *world);
void nextStep(world_t *world);
void bwait();
_Bool read_world(world_t *world, char *path);

/*
 * Main
 */
int main(int argc, char *argv[]) {
  // Parse command line
  if(!optparse(argc, argv))
    abort();

  /* Nanosleep Setup */
  int milisec = 250; // length of time to sleep, in miliseconds
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = milisec * 1000000L;

  // Initialisieren der Welt und den Zellen
  world_t world;

  /* Allocate Memory */
  allocateMemory(&world);

  /* Welt befüllen */
  if(path) {
    read_world(&world, path);
  } else if (!create_world(&world)) {
      printf("error creating world\n");
      exit(-2);
  } else {
    fillWorld(&world);
  }

  /* Welt ausgeben */
  printWorld(&world);

  /*
   * Nächster Schritt ausführen
   * TODO: Beenden der Schleife wenn sich nichts mehr ändert
   */
  while (true) {
    (automode) ? nanosleep(&req, (struct timespec *)NULL) : bwait();
    nextStep(&world);
    printWorld(&world);
  }
  return 0;
}

void allocateMemory(world_t *world) {
  // Es wird eine Spalte Memory alloziert
  world->cells = (cell_t**)malloc(height * sizeof(cell_t*));

  // Und jetzt das ganze für jede Zeile
  int i;
  for(i=0;i<height;i++) {
    world->cells[i] = (cell_t*)malloc(width * sizeof(cell_t*));
  }
}

void nextStep(world_t *world) {
  int neighbours=0;
  int i,j;
  for (i=0; i < height; i++) {
    for (j=0; j<width; j++) {
      neighbours = countNeighbours(i,j,world);
      // Regeln
      if(neighbours < 2 || neighbours > 3) world->cells[i][j].alive_next_round = false;
      if(world->cells[i][j].alive && neighbours == 2) world->cells[i][j].alive = true;
      if(neighbours == 3) world->cells[i][j].alive_next_round = true;
    }
  }

  for(i=0; i<height; i++) {
    for (j=0; j<width; j++) {
      world->cells[i][j].alive = (world->cells[i][j].alive_next_round) ? true : false;
    }
  }
}

void printWorld(world_t *world) {
  int i,j;
  for (i=0; i < height; i++) {
    for (j=0; j<width; j++) {
      if(world->cells[i][j].alive) {
        //printf("●");
        //printf("○");
        //printf("◆");
        printf("☀");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

_Bool fillWorld(world_t *world) {
  int i,j;
  // Random seed generieren
  srand(time(NULL));
  // Welt befüllen
  for (i=0;i<height; i++) {
    for (j=0; j<width; j++) {
      world->cells[i][j].alive = ((rand() % 2) == 0) ? true : false;
    }
  }
}

_Bool create_world(world_t *world) {
  int i;

  // allocate memory to hold all lines in the world
  world->cells = (cell_t **) malloc(sizeof (cell_t *) * height);

  // for each line in the world, assign memory to it
  for(i = 0; i<=height; i++) {
    if(!(world->cells[i] = (cell_t *) malloc(sizeof(cell_t *) * width)))
      return false;
  }

  return true;
}


int countNeighbours(int x, int y, world_t *world) {
  int counter = 0;

  /*
     -------->x
     | a b c
     | d ● e
     | f g h
     y
  */

  // a = [x-1][y-1]
  if(x>=1 && y>=1) {
    if(world->cells[x-1][y-1].alive) counter++;
  }
  // b = [x]  [y-1]
  if(y>=1) {
    if(world->cells[x][y-1].alive) counter++;
  }
  // c = [x+1][y-1]
  if(x<(height-1) && y>=1) {
    if(world->cells[x+1][y-1].alive) counter++;
  }
  // d = [x-1][y]
  if (x>=1) {
    if(world->cells[x-1][y].alive) counter++;
  }
  // e = [x+1][y]
  if (x<(height-1)) {
    if(world->cells[x+1][y].alive) counter++;
  }
  // f = [x-1][y+1]
  if(x>=1 && y<(width-1)) {
    if(world->cells[x-1][y+1].alive) counter ++;
  }
  // g = [x]  [y+1]
  if(y<(width-1)) {
    if(world->cells[x][y+1].alive) counter ++;
  }
  // h = [x+1][y+1]
  if(x<(height-1) && y<(width-1)) {
    if(world->cells[x+1][y+1].alive) counter++;
  }
  return counter;
}

void bwait() {
  printf("Press Enter to continue...\n");
  getchar();
}

_Bool read_world(world_t *world, char *path) {
  long file_size = 0;
  FILE *fp;
  char *input = NULL;

  fp = fopen(path, "r");

  // get filesize
  fseek(fp, 0L, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // read file
  input = (char *)malloc(sizeof(char) * file_size);
  fread(input, file_size, sizeof(char), fp);

  // note: there is no syntax checking
  width = strstr(input, "\n") - input;
  // account newlines (+1)
  height = (file_size) / (width + 1);

  create_world(world);

  int i, j, tmp = 0;
  for(i = 0; i<height; i++) {
    for(j = 0; j<width; j++) {
      world->cells[i][j].alive = (input[tmp] == '*') ? true : false;
      tmp += 1;
    }
    tmp += 1;
  }

  return true;
}

