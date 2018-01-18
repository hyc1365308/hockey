#include "back.h"

double ballX = 0;
double ballY = 0;
double playerX = 0;
double playerY = 0;
double AIX = 0;
double AIY = 0;
double tableX = 0;
double tableY = 0;
double ballSpeed = 0;
double ballRadium = 0;
double malletRadium = 0;
double ballXspeed = 0;
double ballYspeed = 0;
double scoreWidth = 0;

int firstInit() {
	tableY = 4 * SIZEFACTOR;
	tableX = 2 * SIZEFACTOR;
	ballX = tableX / 2;
	ballY = tableY / 2;
	ballSpeed = 0.01 * SIZEFACTOR;
	ballRadium = 0.1 * SIZEFACTOR;
	malletRadium = 0.15 * SIZEFACTOR;
	ballXspeed = 1 * SIZEFACTOR;
	ballYspeed = 0 * SIZEFACTOR;
	scoreWidth = 0.8 * SIZEFACTOR;
	return 1;
}

int update() {
	ballX += ballXspeed;
	ballY += ballYspeed;
	int status = 0;
	if (status = ballKnockWall()) {
		if (scored()) {
			if (ballY < tableY / 2) return AIWIN;
			else return PLAYERWIN;
		}
		else {
			if (status == 1) {
				ballX -= ballXspeed;
				ballXspeed = -ballXspeed;
				ballX += ballXspeed;
			}
			else {
				ballY -= ballYspeed;
				ballYspeed = -ballYspeed;
				ballY += ballYspeed;
			}
		}
		return CONTINUE;
	}
	if (status = ballKnockMallet()) {

	}

}

int ballKnockWall() {
	if (ballX + ballRadium >= tableX || ballX - ballRadium <= 0)
		return 1;
	if (ballY + ballRadium >= tableY || ballY - ballRadium <= 0)
		return 2;
	return 0;
}

int scored() {
	if ((tableX / 2 + scoreWidth / 2 >= ballX) && (tableX / 2 - scoreWidth / 2 <= ballX)) return 1;
	return 0;
}

int ballKnockMallet() {
	return 0;
}

double dist(double x1, double x2, double y1, double y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
