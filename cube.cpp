#define WindowWidth  600  
#define WindowHeight 500  
#define WindowTitle  "Hockey"  

#include <glut.h>  
#include <windows.h>  
#include <stdio.h>  
#include <stdlib.h> 
#include <math.h>
#include "back.h"

//�����������������  
GLuint texGround;
GLuint texWall;

#define BMP_Header_Length 54  //ͼ���������ڴ���е�ƫ����  
static GLfloat angle = 0.0f;   //��ת�Ƕ�  
const GLfloat pi = 3.1415926536f;

int tablepos[4][2] = {
	112, 316,
	487, 316,
	244, 116,
	355, 116,
};
int mid = 200;

// ����power_of_two�����ж�һ�������ǲ���2����������  
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

GLuint TextFont;
void XPrintString(char *s)
{
	glPushAttrib(GL_LIST_BIT);

	//����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
	for (; *s != '\0'; ++s)
		glCallList(TextFont + *s);

	glPopAttrib();
}

/* ����load_texture
* ��ȡһ��BMP�ļ���Ϊ����
* ���ʧ�ܣ�����0������ɹ�������������
*/
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// ���ļ������ʧ�ܣ�����  
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�  
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���  
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�  
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������  
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������  
	// ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����  
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������  
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���  
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�  
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������  
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height  
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������  
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// ���µ������������������������  
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�  
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������  
	free(pixels);
	return texture_ID;
}

void init() {
	firstInit();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//����MAX_CHAR����������ʾ�б���
	TextFont = glGenLists(128);

	//��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, TextFont);
}

void draw_table_sides_1(int i) {
	//������
	glColor3f(0.4, 0.4, 0.4);
	glVertex3f(-2.4 * i, 6.0, 12.6);
	glVertex3f(-2.4 * i, 6.0, 12.0);
	glVertex3f(-2.4 * i, 6.6, 12.0);
	glVertex3f(-2.4 * i, 6.6, 12.6); // ���űߵ�С��

	glVertex3f(-2.4 * i, 6.0, 12.6);
	glVertex3f(-2.4 * i, 6.6, 12.6);
	glVertex3f(-6.6 * i, 6.6, 12.6);
	glVertex3f(-6.6 * i, 6.0, 12.6); // ����Χ˳��һ������

	glVertex3f(-6.6 * i, 6.0, 12.6);
	glVertex3f(-6.6 * i, 6.6, 12.6);
	glVertex3f(-6.6 * i, 6.6, -12.6);
	glVertex3f(-6.6 * i, 6.0, -12.6);

	glVertex3f(-2.4 * i, 6.0, -12.6);
	glVertex3f(-2.4 * i, 6.6, -12.6);
	glVertex3f(-6.6 * i, 6.6, -12.6);
	glVertex3f(-6.6 * i, 6.0, -12.6);

	glVertex3f(-2.4 * i, 6.0, -12.6);
	glVertex3f(-2.4 * i, 6.0, -12.0);
	glVertex3f(-2.4 * i, 6.6, -12.0);
	glVertex3f(-2.4 * i, 6.6, -12.6);

	glVertex3f(-2.4 * i, 6.0, -12.0);
	glVertex3f(-2.4 * i, 6.6, -12.0);
	glVertex3f(-6.0 * i, 6.6, -12.0);
	glVertex3f(-6.0 * i, 6.0, -12.0);

	glVertex3f(-6.0 * i, 6.0, 12.0);
	glVertex3f(-6.0 * i, 6.6, 12.0);
	glVertex3f(-6.0 * i, 6.6, -12.0);
	glVertex3f(-6.0 * i, 6.0, -12.0);

	glVertex3f(-2.4 * i, 6.0, 12.0);
	glVertex3f(-2.4 * i, 6.6, 12.0);
	glVertex3f(-6.0 * i, 6.6, 12.0);
	glVertex3f(-6.0 * i, 6.0, 12.0);

	// ������
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(-2.4 * i, 6.6, 12.6);
	glVertex3f(-2.4 * i, 6.6, 12.0);
	glVertex3f(-6.6 * i, 6.6, 12.0);
	glVertex3f(-6.6 * i, 6.6, 12.6);

	glVertex3f(-2.4 * i, 6.6, -12.6);
	glVertex3f(-2.4 * i, 6.6, -12.0);
	glVertex3f(-6.6 * i, 6.6, -12.0);
	glVertex3f(-6.6 * i, 6.6, -12.6);

	glVertex3f(-6.0 * i, 6.6, -12.0);
	glVertex3f(-6.0 * i, 6.6, 12.0);
	glVertex3f(-6.6 * i, 6.6, 12.0);
	glVertex3f(-6.6 * i, 6.6, -12.0);

}

void draw_table() {
	GLfloat table_list[][3] = {
		-6.4f, 0.0f, -12.4f,
		6.4f, 0.0f, -12.4f,
		6.4f, 0.0f, 12.4f,
		-6.4f, 0.0f, 12.4f,
		-6.4f, 6.0f, -12.4f,
		6.4f, 6.0f, -12.4f,
		6.4f, 6.0f, 12.4f,
		-6.4f, 6.0f, 12.4f,
	};
	GLint index_list[][4] = {
		0, 1, 5, 4,
		1, 2, 6, 5,
		2, 3, 7, 6,
		3, 0, 4, 7,
	};
	glBegin(GL_QUADS); // �����ı���
	glColor3f(0.4, 0.4, 0.4);
	for (int i = 0; i < 4; ++i)         // ��4���棬ѭ��5��
	{
		for (int j = 0; j < 4; ++j)     // ÿ�������ĸ����㣬ѭ���Ĵ�
		{
			glVertex3fv(table_list[index_list[i][j]]);
		}
	}
	//���ŵ�������
	glVertex3f(-2.4, 6.0, 12.4);
	glVertex3f(-2.4, 6.0, 12.0);
	glVertex3f(2.4, 6.0, 12.0);
	glVertex3f(2.4, 6.0, 12.4);

	glVertex3f(-2.4, 6.0, -12.4);
	glVertex3f(-2.4, 6.0, -12.0);
	glVertex3f(2.4, 6.0, -12.0);
	glVertex3f(2.4, 6.0, -12.4);

    // ����
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(-6.0, 6.0, -12.0);
	glVertex3f(-6.0, 6.0, 12.0);
	glVertex3f(6.0, 6.0, 12.0);
	glVertex3f(6.0, 6.0, -12.0);

	//������
	draw_table_sides_1(1);
	draw_table_sides_1(-1);

	glEnd();
}

void draw_puck() {

	glPushMatrix();
	glTranslatef(0.0, 6.6, 0.0);
	glTranslatef(-6.0 + ballX, 0.0, 12.0 - ballY);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 0.0f, 0.0f);
	GLUquadricObj *objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, 0.6, 0.6, 0.6, 32, 5);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 40; i++)
	{
		glVertex3f(0.6 * cos(2 * pi / 40*i), 0.6 * sin(2 * pi / 40*i), 0.0);
	}
	glEnd();

	glPopMatrix();
}

void draw_mallet_self() {

	glPushMatrix();
	glTranslatef(0.0, 6.6, 0.0);
	glTranslatef(playerX - 6.0, 0.0, 12.0 - playerY);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	GLUquadricObj *objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, 0.9, 0.9, 0.6, 32, 5);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 40; i++)
	{
		glVertex3f(0.9 * cos(2 * pi / 40 * i), 0.9 * sin(2 * pi / 40 * i), 0.0);
	}
	glEnd();

	glPopMatrix();
}

void draw_mallet_comp() {

	glPushMatrix();
	glTranslatef(0.0, 6.6, 0.0);
	glTranslatef(AIX - 6.0, 0.0, 12.0 - AIY);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 0.0f, 1.0f);
	GLUquadricObj *objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, 0.9, 0.9, 0.6, 32, 5);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 40; i++)
	{
		glVertex3f(0.9 * cos(2 * pi / 40 * i), 0.9 * sin(2 * pi / 40 * i), 0.0);
	}
	glEnd();

	glPopMatrix();
}

void display(void)
{
	// �����Ļ  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// �����ӽ�  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, 1.5, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 12, 18, 0, 0, 0, 0, 1, 0);

	glRotatef(angle, 0.0f, 1.0f, 0.0f); //��ת  

										// ���Ƶ����Լ�����  
	glBindTexture(GL_TEXTURE_2D, texGround);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 0.0f, -20.0f);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-20.0f, 0.0f, 20.0f);
	glTexCoord2f(3.0f, 3.0f); glVertex3f(20.0f, 0.0f, 20.0f);
	glTexCoord2f(3.0f, 0.0f); glVertex3f(20.0f, 0.0f, -20.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	draw_table();

	draw_puck();
	draw_mallet_self();
	draw_mallet_comp();
}

void display_ctl(void) {
	int code = update();
	display();
	if (code == 2) {
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos3f(0.0, 8.0, 0.0);  //��ʼλ��  
		XPrintString("You win!");
	}
	else if (code == 3) {
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos3f(0.0, 8.0, 0.0);  //��ʼλ��  
		XPrintString("You lose!");
	}
	glutSwapBuffers();
}

void gameReset() {
	printf("game reset!\n");
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		if (angle >= 60 && angle <= 180) break;
		angle = (angle + 2.0);
		if (angle >= 360.0f) angle -= 360.0f;
		break;
	case 'd':
		if (angle >= 180 && angle <= 300) break;
		angle = (angle - 2.0);
		if (angle <= 0.0f) angle += 360.0f;
		break;
	case char(13):
		gameReset();
	}
}

void mouse(int x, int y) {
	printf("%d, %d\n", x, y);
	//���ж��Ƿ��ڹ涨������
	if (y > tablepos[0][1]) {
		y = tablepos[0][1];
	}
	if (y < tablepos[2][1]) {
		y = tablepos[2][1];
	}
	double k1 = - 200.0 / 132.0;
	double b1 = tablepos[0][1] - k1 * tablepos[0][0];
	double k2 = -k1;
	double b2 = tablepos[1][1] - k2 * tablepos[1][0];

	double normal_x, normal_y;/*
	if (y - k1 * x - b1 < 0) {
		if (y - k2 * x - b2 < 0) {
			// �������ϱ�����
			// ֱ�ӹ̶����ϲ��м�
			normal_x = 6.0;
			normal_y = 11.0;
		}
		else {
			normal_x = 12.0 - 0.9;
			normal_y = 11.0;
		}
	}
	else {
		if (y - k2 * x - b2 < 0) {
			// �������ϱ�����
			// ֱ�ӹ̶����ϲ��ұ�
			normal_x = 0.9;
			normal_y = 11.0;
		}
		else {
			if (y < 250) {
				//�ڶԷ��볡
				normal_x = 6.0;
				normal_y = 11.0;
			}else if (y > 316) {
				//���ҷ��볡����
				normal_x = 6.0;
				normal_y = 0.9;
			}else {
				//����λ��
				normal_y = (316 - y) / 200.0 * 24.0;
				double x_left = (y - b1) / k1;
				double x_right = (y - b2) / k2;
				normal_x = (x - x_left) / (x_right - x_left) * 12.0;
			}
		}
	}*/
	if (y - k1 * x - b1 > 0) {
		if (y - k2 * x - b2 > 0) {
			if (y >= 250 && y <= 316) {
				//����λ��
				normal_y = (316 - y) / 200.0 * 24.0;
				double x_left = (y - b1) / k1;
				double x_right = (y - b2) / k2;
				normal_x = (x - x_left) / (x_right - x_left) * 12.0;
				normal_x = __min(__max(normal_x, 0.9), 11.1);
				normal_y = __min(__max(normal_y, 0.9), 23.1);
				printf("%lf, %lf\n", normal_x, normal_y);
				playerX = normal_x;
				playerY = normal_y;
			}
		}
	}
	
}

int main(int argc, char* argv[])
{
	// GLUT��ʼ��  
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 200);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WindowTitle);
	init();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);    // ��������  
	texGround = load_texture(".\\floor.bmp");  //��������  
	printf("%d\n", texGround);
	glutDisplayFunc(&display_ctl);   //ע�ắ��   
	glutIdleFunc(&display_ctl);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutMainLoop(); //ѭ������  
	return 0;
}