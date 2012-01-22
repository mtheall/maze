#include <stdio.h>
#include <feos.h>
#include <feos3d.h>
#include <stdlib.h>
#include "fixed.h"
#include "maze.h"

void init3D() {
  FeOS_DirectMode();

  videoSetMode(MODE_0_3D);
  videoSetModeSub(MODE_0_2D);
  vramSetPrimaryBanks(VRAM_A_LCD,
                      VRAM_B_LCD,
                      VRAM_C_LCD,
                      VRAM_D_LCD);

  glInit();
  glEnable(GL_ANTIALIAS);

  glSetOutlineColor(1, RGB15(0,0,0));
  glClearColor(0,0,24,31);
  glClearPolyID(0);
  glClearDepth(0x7FFF);
  glPolyFmt(POLY_ID(1) | POLY_ALPHA(31) | POLY_CULL_NONE);

  glViewport(0,0,255,191);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspectivef32(degreesToAngle(60),
                    floattof32(256.0f/192.0f),
                    floattof32(0.1f),
                    floattof32(100.0f));
  gluLookAt(floattof32(0.0f), floattof32(0.0f), floattof32(1.0f),
            floattof32(0.0f), floattof32(0.0f), floattof32(0.0f),
            floattof32(0.0f), floattof32(1.0f), floattof32(0.0f));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void drawMaze(Maze *maze, int x, int y) {
  int i, j;
  int rows = maze->getRows();
  int cols = maze->getCols();
  int walls;

  glPushMatrix();
  for(j = y-5; j < y+5; j++) {
    if(j < 0 || j >= rows)
      continue;
    for(i = x-5; i < x+5; i++) {
      if(i < 0 || i >= cols)
        continue;

      f32 x1 = f32(inttof32(i-x))  /f32(inttof32(2)) - f32(floattof32(0.25f));
      f32 x2 = f32(inttof32(i-x+1))/f32(inttof32(2)) - f32(floattof32(0.25f));
      f32 y1 = f32(inttof32(j-y))  /f32(inttof32(2)) - f32(floattof32(0.25f));
      f32 y2 = f32(inttof32(j-y+1))/f32(inttof32(2)) - f32(floattof32(0.25f));

      glColor(RGB15(y-j+15, y-j+15, y-j+15));
      glBegin(GL_QUAD);
        walls = maze->getWalls(i, j);
        if(j == 0 || (j == y-5 && (walls & WALL_UP))) {
          glVertex3v16(x1.getValue(), -y1.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y1.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y1.getValue(), floattof32(-0.5f));
          glVertex3v16(x1.getValue(), -y1.getValue(), floattof32(-0.5f));
        }
        if(walls & WALL_DOWN) {
          glVertex3v16(x1.getValue(), -y2.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y2.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y2.getValue(), floattof32(-0.5f));
          glVertex3v16(x1.getValue(), -y2.getValue(), floattof32(-0.5f));
        }
        if(i == 0 || (i == x-5 && (walls & WALL_LEFT))) {
          glVertex3v16(x1.getValue(), -y1.getValue(), floattof32(-1.0f));
          glVertex3v16(x1.getValue(), -y2.getValue(), floattof32(-1.0f));
          glVertex3v16(x1.getValue(), -y2.getValue(), floattof32(-0.5f));
          glVertex3v16(x1.getValue(), -y1.getValue(), floattof32(-0.5f));
        }
        if(walls & WALL_RIGHT) {
          glVertex3v16(x2.getValue(), -y1.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y2.getValue(), floattof32(-1.0f));
          glVertex3v16(x2.getValue(), -y2.getValue(), floattof32(-0.5f));
          glVertex3v16(x2.getValue(), -y1.getValue(), floattof32(-0.5f));
        }
      glEnd();

      if(i == x && j == y)
        glColor(RGB15( 0,  0, 31));
      else if(maze->getTraveled(i, j))
        glColor(RGB15( 0, 31,  0));
      else
        glColor(RGB15( 5,  5,  5));

      glBegin(GL_QUAD);
        glVertex3v16(x1.getValue(), -y1.getValue(), floattof32(-1.0f));
        glVertex3v16(x2.getValue(), -y1.getValue(), floattof32(-1.0f));
        glVertex3v16(x2.getValue(), -y2.getValue(), floattof32(-1.0f));
        glVertex3v16(x1.getValue(), -y2.getValue(), floattof32(-1.0f));
      glEnd();
    }
  }
  glPopMatrix(1);
}

int main(int argc, char *argv[]) {
  Maze *maze;
  int  threads = 1;
  int x = 0, y = 0;
  int down;
  int walls;

  if(argc > 1)
    threads = atoi(argv[1]);
  if(threads == 0 || threads < 0)
    threads = 1;

  init3D();
  maze = new Maze(32, 32, threads);

  walls = maze->getWalls(x, y);
  maze->setTraveled(x, y);

  keysSetRepeat(12, 12);
  while(1) {
    swiWaitForVBlank();

    down = keysDownRepeat();

    if((down & KEY_UP) && (walls & WALL_UP) == 0)
      y--;
    if((down & KEY_DOWN) && (walls & WALL_DOWN) == 0)
      y++;
    if((down & KEY_LEFT) && (walls & WALL_LEFT) == 0)
      x--;
    if((down & KEY_RIGHT) && (walls & WALL_RIGHT) == 0)
      x++;

    if(down & (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)) {
      walls = maze->getWalls(x, y);
      maze->setTraveled(x, y);
    }

    glPushMatrix();
    glTranslatef32(0, 0, floattof32(-1.0f));
    glRotateXi(degreesToAngle(-30));
    drawMaze(maze, x, y);
    glPopMatrix(1);
    glFlush(0);
  }

  delete maze;

  return 0;
}

