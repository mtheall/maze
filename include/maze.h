#pragma once
#ifndef MAZE_H
#define MAZE_H

#define WALL_UP    (1<<0)
#define WALL_DOWN  (1<<1)
#define WALL_LEFT  (1<<2)
#define WALL_RIGHT (1<<3)

struct cell_t {
  int walls;
  int group;
};

class Maze {
private:
  cell_t *maze;
  int    rows;
  int    cols;
  bool   finished();
  int    teleport(int group);
public:
  Maze(int rows, int cols, int numThreads = 1);
  ~Maze();

  int worker();
  int getWalls(int x, int y);

  void setTraveled(int x, int y);
  bool getTraveled(int x, int y);

  int getRows() const { return rows; }
  int getCols() const { return cols; }
};

#endif /* MAZE_H */

