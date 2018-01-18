#include "back.h"

double ballX = 0;
double ballY = 0;
double playerX = 0;
double playerY = 0;
double AIX = 0;
double AIY = 0;
double AISpeed = 0;
double tableX = 0;
double tableY = 0;
double ballSpeed = 0;
double ballRadium = 0;
double malletRadium = 0;
double ballXspeed = 0;
double ballYspeed = 0;
double scoreWidth = 0;

int firstInit() {
	tableX = 2 * SIZEFACTOR;
	tableY = 4 * SIZEFACTOR;	
	ballX = tableX / 2;
	ballY = tableY / 2;
	AIX = tableX / 2;
	AIY = tableY / 4 * 3;
	playerX = tableX / 2;
	playerY = tableY / 4;
	ballSpeed = 0.01 * SIZEFACTOR;
	AISpeed = 0.001 * SIZEFACTOR;
	ballRadium = 0.1 * SIZEFACTOR;
	malletRadium = 0.15 * SIZEFACTOR;
	ballXspeed = 0;
	ballYspeed = 0.01 * SIZEFACTOR;
	scoreWidth = 0.8 * SIZEFACTOR;
	return 1;
}

int update() {
	AIX += AISpeed;
	if (AIX <= malletRadium) AIX -= AISpeed;
	if (AIX >= tableX - malletRadium) AIX -= AISpeed;
	if (ballX - AIX > 0) AISpeed = abs(AISpeed);
	if (ballX - AIX < 0) AISpeed = -abs(AISpeed);
	
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
		if (status == 1) {
			ballX -= ballXspeed;
			ballY -= ballYspeed;
			ballXspeed = ballSpeed * (ballX - playerX) / (ballRadium + malletRadium);
			ballYspeed = ballSpeed * (ballY - playerY) / (ballRadium + malletRadium);
			ballX += ballXspeed;
			ballY += ballYspeed;
			return CONTINUE;
		}
		if (status == 2) {
			ballX -= ballXspeed;
			ballY -= ballYspeed;
			ballXspeed = ballSpeed * (ballX - AIX) / (ballRadium + malletRadium);
			ballYspeed = ballSpeed * (ballY - AIY) / (ballRadium + malletRadium);
			ballX += ballXspeed;
			ballY += ballYspeed;
			return CONTINUE;
		}
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
	if (dist(ballX, playerX, ballY, playerY) <= ballRadium + malletRadium)
		return 1;
	if (dist(ballX, AIX, ballY, AIY) <= ballRadium + malletRadium)
		return 2; 
	return 0;
}

double dist(double x1, double x2, double y1, double y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
