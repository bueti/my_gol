#include <stdlib.h>
#include <stdio.h>

#include "optparse.h"

int height, width = 0;
char *path        = NULL;

typedef struct {
  _Bool alive;
  _Bool alive_next_round;
} cell_t;

typedef struct {
  cell_t **cells;
} world_t;

int countNeighbours(int x, int y, world_t *world);
void fillWorld(world_t *world);
void printWorld(world_t *world);
void nextStep(world_t *world);
void bwait();

int main(int argc, char *argv[]) {
  if(!optparse(argc, argv))
    abort();

  // Initialisieren der Welt und den Zellen
  world_t world;

  // Es wird eine Spalte Memory alloziert
  // Eigentlich müsste der Zugriff über world->cells erfolgen, da cells ein
  // Pointer ist, interessanterweise funktioniert das aber nicht im main?!
  world.cells = (cell_t**)malloc(height * sizeof(cell_t*));

  // Und jetzt das ganze für jede Zeile
  int i;
  for(i=0;i<height;i++) {
    world.cells[i] = (cell_t*)malloc(width * sizeof(cell_t*));
  }

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
   */
  while(true) {
    bwait();
    nextStep(&world);
    printWorld(&world);
  }

  return 0;
}

void nextStep(world_t *world) {
  int neighbours=0;
  int i,j;
  for (i=0; i < height; i++) {
    for (j=0; j<width; j++) {
      neighbours = countNeighbours(i,j,world);
      // Regel 2-4
      if(neighbours < 2) world->cells[i][j].alive_next_round = false;
      if(neighbours > 3) world->cells[i][j].alive_next_round = false;
      if(neighbours == 3) world->cells[i][j].alive_next_round = true;
      // Regel 1
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

int countNeighbours(int x, int y, world_t *world) {
  int counter = 0;

  // Rand unten links
  if(x<1 && y<1) {
    if (world->cells[x][y+1].alive)    counter++;
    if (world->cells[x+1][y].alive)    counter++;
    if (world->cells[x+1][y+1].alive)  counter++;

    return counter;
  }

  // Rand links
  if(x<1 && y>=1) {
    if (world->cells[x][y+1].alive)    counter++;
    if (world->cells[x][y-1].alive)    counter++;
    if (world->cells[x+1][y].alive)    counter++;
    if (world->cells[x+1][y+1].alive)  counter++;
    if (world->cells[x+1][y-1].alive)  counter++;

    return counter;
  }

  // Rand unten
  if(x>=1 && x<(width-1) && y<1) {
    if (world->cells[x][y+1].alive)    counter++;
    if (world->cells[x+1][y+1].alive)  counter++;
    if (world->cells[x+1][y].alive)    counter++;

    return counter;
  }

  // Rand oben
  if(x == width && y < height) {
    if (world->cells[x][y-1].alive)    counter++;
    if (world->cells[x][y+1].alive)    counter++;
    if (world->cells[x-1][y].alive)    counter++;
    if (world->cells[x-1][y+1].alive)  counter++;
    if (world->cells[x-1][y-1].alive)  counter++;

    return counter;
  }

  // Rand oben rechts
  if(x == width && y == height) {
    if (world->cells[x-1][y].alive)    counter++;
    if (world->cells[x-1][y-1].alive)  counter++;
    if (world->cells[x][y-1].alive)    counter++;

    return counter;
  }

  // Untere Zeile
  if(x>0 && x<(width-1) && y>0 && y<(height-1)) {
    if (world->cells[x-1][y-1].alive)  counter++;
    if (world->cells[x][y-1].alive)    counter++;
    if (world->cells[x+1][y-1].alive)  counter++;

    // Mittlere Zeile
    if (world->cells[x-1][y].alive)    counter++;
    if (world->cells[x+1][y].alive)    counter++;

    // Obere Zeile
    if (world->cells[x-1][y+1].alive)  counter++;
    if (world->cells[x][y+1].alive)    counter++;
    if (world->cells[x+1][y+1].alive)  counter++;

    return counter;
  }
}

void bwait() {
  fflush(stdin);
  printf("\nPress Enter to continue...\n");
  getchar();
}
