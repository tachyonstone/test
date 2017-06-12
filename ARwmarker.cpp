#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>

#include <math.h>

#define WITHIN 200
#define WITHIN2 40

#define OBJECT_SIZE 80.0

char *vconf_name = "Data/WDM_camera_flipV.xml";
char *cparam_name = "Data/camera_para.dat";
char *patt_name1 = "Data/patt.marker1";
char *patt_name2 = "Data/patt.marker2";

int patt_id1;
double patt_trans1[3][4];
double patt_center1[2] = { 0.0, 0.0 };
double patt_width1 = 80.0;

int patt_id2;
double patt_trans2[3][4];
double patt_center2[2] = { 0.0, 0.0 };
double patt_width2 = 80.0;
int thresh = 100;

void MainLoop(void);
void KeyEvent(unsigned char key, int x, int y);
void MouseEvent(int button, int state, int x, int y);
void Cleanup(void);
void DrawObject1(void);
void DrawObject2(void);
void DrawObjectChange(double , double);


int visible1;
int visible2;
double wmat1[3][4];
double wmat2[3][4];

double trans_x=0;
double trans_y=0;

double distance_x = 2000;
double distance_y = 2000;
double distance_tmp;
double isDistance;

int main(int argc, char **argv)
{
	ARParam cparam;
	ARParam wparam;
	int xsize, ysize;

	printf("test\n");

	glutInit(&argc, argv);

	if (arVideoOpen(vconf_name) < 0) {
		printf("ビデオデバイスのエラー");
		return -1;

	}

	if (arVideoInqSize(&xsize, &ysize) < 0)	return -1;

	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("カメラパラメータの読み込みに失敗しました\n");
		return -1;

	}

	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	if ((patt_id1 = arLoadPatt(patt_name1)) < 0) {
		printf("パターンファイルの読み込みに失敗しました\n");
		return -1;
	}

	if ((patt_id2 = arLoadPatt(patt_name2)) < 0) {
		printf("パターンファイルの読み込みに失敗しました\n");
		return -1;
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0);

	arVideoCapStart();

	argMainLoop(MouseEvent, KeyEvent, MainLoop);


	return 0;

}


void MainLoop(void)
{
	ARUint8 *image;
	ARMarkerInfo *marker_info;
	int marker_num;
	int j, k;

	visible1 = 0;
	visible2 = 0;

	if ((image = (ARUint8 *)arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;

	}

	argDrawMode2D();
	argDispImage(image, 0, 0);

	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);

	}

	arVideoCapNext();

	k = -1;
	for (j = 0; j < marker_num; j++) {
		if (patt_id1 == marker_info[j].id) {
			if (k == -1) k = j;
			else if (marker_info[k].cf < marker_info[j].cf) k = j;

		}

	}

	if (k != -1) {
		arGetTransMat(&marker_info[k], patt_center1, patt_width1, patt_trans1);

		//debug
		//printf("%f, %f %f %f\n", marker_info[k], patt_center, patt_width, patt_trans[0][0]);

		DrawObject1();
		visible1 = 1;
	}


	k = -1;
	for (j = 0; j < marker_num; j++) {
		if (patt_id2 == marker_info[j].id) {
			if (k == -1) k = j;
			else if (marker_info[k].cf < marker_info[j].cf) k = j;

		}

	}

	if (k != -1) {
		arGetTransMat(&marker_info[k], patt_center2, patt_width2, patt_trans2);

		//debug
		//printf("%f, %f %f %f\n", marker_info[k], patt_center, patt_width, patt_trans[0][0]);

		if (abs(distance_x) < WITHIN && abs(distance_y) < WITHIN) {

			DrawObjectChange(trans_x,trans_y);
		}
		else {

			DrawObject2();
			printf("dist2: %f\n", distance_x);

		}
		visible2 = 1;
	}

	argSwapBuffers();


	if (visible1 > 0 && visible2 > 0) {
		// マーカー1の座標系でカメラの位置を取得　
		arUtilMatInv(patt_trans1, wmat1);
		// マーカー1から見たマーカー2の位置を取得　
		arUtilMatMul(wmat1, patt_trans2, wmat2);
		printf("%5.3f %5.3f %5.3f\n", wmat2[0][3], wmat2[1][3], wmat2[2][3]);

		//distance = sqrt(pow(wmat2[0][3]+trans_x, 2) + pow(wmat2[1][3]+trans_y, 2) + pow(wmat2[2][2], 2)) / 10;
		//isDistance = distance - distance_tmp;

		distance_x = wmat2[0][3] + trans_x;
		distance_y = wmat2[1][3] + trans_y;

		printf("wmat2[0][3] : %f, wmat2[1][3] : %f\n", wmat2[0][3], wmat2[1][3]);
		if ((0<distance_x && distance_x < WITHIN) && abs(distance_y < WITHIN) && (abs(distance_y) < WITHIN2)) {
			trans_x += WITHIN - distance_x;
			DrawObjectChange(trans_x, trans_y);

		}
		else if ((-WITHIN < distance_x && distance_x < 0) && (abs(distance_y) < WITHIN) && (abs(distance_y) < WITHIN2)) {
			trans_x = trans_x - (WITHIN + distance_x);
			DrawObjectChange(trans_x, trans_y);
		}
		else if ((0<distance_y && distance_y < WITHIN) && abs(distance_x) < WITHIN && abs(distance_x) < WITHIN2) {
			trans_y += WITHIN - distance_y;
			DrawObjectChange(trans_x, trans_y);

		}
		else if ((-WITHIN < distance_y && distance_y < 0) && (abs(distance_x) < WITHIN) && abs(distance_x) < WITHIN2) {
			trans_y = trans_y - (WITHIN + distance_y);
			DrawObjectChange(trans_x, trans_y);
		}

		printf("distance_y:%f\n", distance_y);

	}
	else {
		DrawObject2();

	}



}


void DrawObject1(void)
{
	double gl_para[16];

	argDrawMode3D();
	argDraw3dCamera(0, 0);

	argConvGlpara(patt_trans1, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	glTranslatef(0.0, 0.0, 20.0);
	glColor3f(0.0, 1.0, 0.0);
	glLineWidth(3.0);
	glutWireCube(OBJECT_SIZE);

}

void DrawObject2(void)
{
	double gl_para[16];

	argDrawMode3D();
	argDraw3dCamera(0, 0);

	argConvGlpara(patt_trans2, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	glTranslatef(trans_x , trans_y, 20.0);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(3.0);
	glutWireCube(OBJECT_SIZE);
	

	/*glPushMatrix();
	glColor3f(0.1f, 0.1f, 1.0f);
	glutSolidCone(10, 150, 10, 10);
	glTranslatef(0.0f, 0.0f, 2.0f);
	glPopMatrix();
	*/
}

void DrawObjectChange(double, double)
{
	double gl_para[16];

	argDrawMode3D();
	argDraw3dCamera(0, 0);


	argConvGlpara(patt_trans2, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	glTranslatef(trans_x, trans_y, 20.0);
	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(3.0);
	glutWireCube(OBJECT_SIZE);

}

void KeyEvent(unsigned char key, int x, int y)
{
	if (key == 0x1b) {
		Cleanup();
		exit(0);

	}
}

void MouseEvent(int button, int state, int x, int y)
{
	printf("ボタン:%d 状態:%d 座標:(x, y) = (%d, %d) \n", button, state, x, y);

}


void Cleanup(void)
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();

}