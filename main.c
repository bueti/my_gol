#include <stdlib.h>
#include <stdio.h>

#include "optparse.h"

/*
 * Global Vars
 */
int height, width = 0;
char *path        = NULL;

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
int countNeighbours(int y, int x, world_t *world);
void fillWorld(world_t *world);
void printWorld(world_t *world);
void nextStep(world_t *world);
void bwait();

/*
 * Main
 */
int main(int argc, char *argv[]) {
  // Parse command line
  if(!optparse(argc, argv))
    abort();

  // Initialisieren der Welt und den Zellen
  world_t world;

  /*
   * Allocate Memory
   */
  allocateMemory(&world);

  /*
   * Welt befüllen
   */
  fillWorld(&world);

  /*
   * Welt ausgeben
   */
  printWorld(&world);

  /*
   * Nächster Schritt ausführen
   * TODO: Auto Mode
   * TODO: Beenden der Schleife wenn sich nichts mehr ändert
   */
  while(true) {
    bwait();
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
      if(neighbours < 2) world->cells[i][j].alive_next_round = false;
      if(neighbours > 3) world->cells[i][j].alive_next_round = false;
      if(neighbours == 3) world->cells[i][j].alive_next_round = true;
      if(world->cells[i][j].alive && neighbours == 2) world->cells[i][j].alive = true;
    }
  }

  for(i=0; i< height; i++) {
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
        printf("●");
        //printf("█");
        //printf("▆");
        //printf("◈");
        //printf("○");
        //printf("x");
      } else {
        printf(" ");
      }
    }
    printf("\n");
  }
}

void fillWorld(world_t *world) {
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

int countNeighbours(int y, int x, world_t *world) {
  int counter = 0;

  // Rand unten links
  if(x<1 && y<1) {
    if (world->cells[y][x+1].alive)    counter++;
    if (world->cells[y+1][x].alive)    counter++;
    if (world->cells[y+1][x+1].alive)  counter++;

    return counter;
  }

  // Rand links
  if(y<1 && x>=1) {
    if (world->cells[y][x+1].alive)    counter++;
    if (world->cells[y][x-1].alive)    counter++;
    if (world->cells[y+1][x].alive)    counter++;
    if (world->cells[y+1][x+1].alive)  counter++;
    if (world->cells[y+1][x-1].alive)  counter++;

    return counter;
  }

  // Rand unten
  // FIXME
  if(y>=1 && y<(height-1) && x<1) {
    if (world->cells[y][x+1].alive)    counter++;
    if (world->cells[y+1][x+1].alive)  counter++;
    if (world->cells[y+1][x].alive)    counter++;

    return counter;
  }

  // Rand oben
  if(y == height && x < width) {
    if (world->cells[y][x-1].alive)    counter++;
    if (world->cells[y][x+1].alive)    counter++;
    if (world->cells[y-1][x].alive)    counter++;
    if (world->cells[y-1][x+1].alive)  counter++;
    if (world->cells[y-1][x-1].alive)  counter++;

    return counter;
  }

  // Rand oben rechts
  if(x == width && y == height) {
    if (world->cells[y-1][x].alive)    counter++;
    if (world->cells[y-1][x-1].alive)  counter++;
    if (world->cells[y][x-1].alive)    counter++;

    return counter;
  }

  // Untere Zeile
  // FIXME
  if(y>0 && y<(height-1) && x>0 && x<(width-1)) {
    if (world->cells[y-1][x-1].alive)  counter++;
    if (world->cells[y][x-1].alive)    counter++;
    if (world->cells[y+1][x-1].alive)  counter++;

    // Mittlere Zeile
    if (world->cells[y-1][x].alive)    counter++;
    if (world->cells[y+1][x].alive)    counter++;

    // Obere Zeile
    if (world->cells[y-1][x+1].alive)  counter++;
    if (world->cells[y][x+1].alive)    counter++;
    if (world->cells[y+1][x+1].alive)  counter++;

    return counter;
  }
}

void bwait() {
  fflush(stdin);
  printf("Press Enter to continue...\n");
  getchar();
}
