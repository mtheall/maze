#include <stdlib.h>
#include <feos.h>
#include <multifeos.h>
#include "maze.h"

#define UNDUG INT_MAX
#define TRAVELED (UNDUG-1)

inline bool Maze::finished() {
  int i;

  for(i = 0; i < rows*cols; i++) {
    if(maze[i].group == UNDUG
    || maze[i].walls == (WALL_UP|WALL_DOWN|WALL_LEFT|WALL_RIGHT)) {
      return false;
    }
  }

  return true;
}

inline int Maze::teleport(int group) {
  int i;
  int cell;
  int row;
  int col;

  while(!finished()) {
    cell = rand()%(rows*cols);
    for(i = 0; i < 20; i++, cell++) {
      if(cell == rows*cols)
        cell = 0;

      if(maze[cell].group == group) {
        row = cell/cols;
        col = cell%cols;

        if(group == UNDUG
        || (row != 0      && maze[cell-cols].group != group)
        || (row != rows-1 && maze[cell+cols].group != group)
        || (col != 0      && maze[cell-1].group    != group)
        || (col != cols+1 && maze[cell+1].group    != group))
          return cell;
      }
    }
    FeOS_Yield();
  }

  return -1;
}

int workhelper(void *p) {
  Maze *maze = (Maze*)p;

  return maze->worker();
}

int Maze::worker() {
  int group;
  int cell;
  int cell2;
  int row;
  int col;
  int wall;
  int walls;

  FeOS_Yield();
  cell = teleport(UNDUG);
  if(cell != -1) {
    group = cell;
    maze[cell].group = group;
  }

  while(cell != -1) {
    row = cell/cols;
    col = cell%cols;
    walls = maze[cell].walls;
    cell2 = -1;

    while(walls) {
      do {
        wall = 1<<(rand()%4);
      }while(!(wall & walls));

      switch(wall) {
        case WALL_UP:
          cell2 = cell-cols;
          if(row == 0)
            walls &= ~WALL_UP;
          else if(maze[cell2].group == group)
            walls &= ~WALL_UP;
          else if(maze[cell2].group == UNDUG) {
            maze[cell2].group = group;
            maze[cell2].walls &= ~WALL_DOWN;
            maze[cell].walls  &= ~WALL_UP;

            cell = cell2;
            walls = 0;
          }
          else {
            int i;
            for(i = 0; i < rows*cols; i++) {
              if(maze[i].group == group)
                maze[i].group = maze[cell2].group;
            }
            maze[cell2].walls &= ~WALL_DOWN;
            maze[cell].walls  &= ~WALL_UP;
            cell = cell2 = teleport(UNDUG);
            if(cell != -1) {
              group = cell;
              maze[cell].group = group;
            }
            walls = 0;
          }
          break;

        case WALL_DOWN:
          cell2 = cell+cols;
          if(row == rows-1)
            walls &= ~WALL_DOWN;
          else if(maze[cell2].group == group)
            walls &= ~WALL_DOWN;
          else if(maze[cell2].group == UNDUG) {
            maze[cell2].group = group;
            maze[cell2].walls &= ~WALL_UP;
            maze[cell].walls  &= ~WALL_DOWN;

            cell = cell2;
            walls = 0;
          }
          else {
            int i;
            for(i = 0; i < rows*cols; i++) {
              if(maze[i].group == group)
                maze[i].group = maze[cell2].group;
            }
            maze[cell2].walls &= ~WALL_UP;
            maze[cell].walls  &= ~WALL_DOWN;
            cell = cell2 = teleport(UNDUG);
            if(cell != -1) {
              group = cell;
              maze[cell].group = group;
            }
            walls = 0;
          }
          break;

        case WALL_LEFT:
          cell2 = cell-1;
          if(col == 0)
            walls &= ~WALL_LEFT;
          else if(maze[cell2].group == group)
            walls &= ~WALL_LEFT;
          else if(maze[cell2].group == UNDUG) {
            maze[cell2].group = group;
            maze[cell2].walls &= ~WALL_RIGHT;
            maze[cell].walls  &= ~WALL_LEFT;

            cell = cell2;
            walls = 0;
          }
          else {
            int i;
            for(i = 0; i < rows*cols; i++) {
              if(maze[i].group == group)
                maze[i].group = maze[cell2].group;
            }
            maze[cell2].walls &= ~WALL_RIGHT;
            maze[cell].walls  &= ~WALL_LEFT;
            cell = cell2 = teleport(UNDUG);
            if(cell != -1) {
              group = cell;
              maze[cell].group = group;
            }
            walls = 0;
          }
          break;

        case WALL_RIGHT:
          cell2 = cell+1;
          if(col == cols-1)
            walls &= ~WALL_RIGHT;
          else if(maze[cell2].group == group)
            walls &= ~WALL_RIGHT;
          else if(maze[cell2].group == UNDUG) {
            maze[cell2].group = group;
            maze[cell2].walls &= ~WALL_LEFT;
            maze[cell].walls  &= ~WALL_RIGHT;

            cell = cell2;
            walls = 0;
          }
          else {
            int i;
            for(i = 0; i < rows*cols; i++) {
              if(maze[i].group == group)
                maze[i].group = maze[cell2].group;
            }
            maze[cell2].walls &= ~WALL_LEFT;
            maze[cell].walls  &= ~WALL_RIGHT;
            cell = cell2 = teleport(UNDUG);
            if(cell != -1) {
              group = cell;
              maze[cell].group = group;
            }
            walls = 0;
          }
          break;
      }
      if(walls == 0 && cell != cell2)
        cell = teleport(group);
    }
  }

  return 0;
}

Maze::Maze(int rows, int cols, int numThreads) : rows(rows), cols(cols) {
  int i;
  thread_t *threads;

  threads = new thread_t[numThreads];
  maze    = new cell_t[rows*cols];

  FeOS_InstallYieldIRQ();

  for(i = 0; i < rows*cols; i++) {
    maze[i].group = UNDUG;
    maze[i].walls = WALL_UP|WALL_DOWN|WALL_LEFT|WALL_RIGHT;
  }

  for(i = 0; i < numThreads; i++)
    threads[i] = FeOS_CreateThread(DEFAULT_STACK_SIZE, workhelper, (void*)this);
  for(i = 0; i < numThreads; i++)
    FeOS_ThreadJoin(threads[i]);

  for(i = 0; i < numThreads; i++)
    FeOS_FreeThread(threads[i]);

  delete [] threads;

  FeOS_UninstallYieldIRQ();
}

Maze::~Maze() {
  delete [] maze;
}

int Maze::getWalls(int x, int y) {
  int cell = y*cols+x;

  if(cell < 0 || cell >= rows*cols)
    return -1;

  return maze[cell].walls;
}

void Maze::setTraveled(int x, int y) {
  int cell = y*cols+x;

  if(cell >= 0 && cell < rows*cols)
    maze[cell].group = TRAVELED;
}

bool Maze::getTraveled(int x, int y) {
  int cell = y*cols+x;

  if(cell < 0 || cell >= rows*cols)
    return false;

  return maze[cell].group == TRAVELED;
}

