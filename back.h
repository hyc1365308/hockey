#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>

const int CONTINUE = 1;
const int PLAYERWIN = 2;
const int AIWIN = 3;
const int SIZEFACTOR = 6;

extern double ballX;
extern double ballY;
extern double playerX;
extern double playerY;
extern double AIX;
extern double AIY;
extern double tableX;
extern double tableY;
extern double ballSpeed;
extern double ballRadium;
extern double malletRadium;
extern double ballXspeed;
extern double ballYspeed;
extern double scoreWidth;

int firstInit();
int update();
int ballKnockWall();
int scored();
int ballKnockMallet();
double dist(double x1, double x2, double y1, double y2);