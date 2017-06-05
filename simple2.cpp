#include <windows.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>



char *vconf_name = "Data/WDM_camera_flipV.xml";
char *cparam_name = "Data/camera_para.dat";
char *patt_name = "Data/patt.hiro";

int patt_id;
double patt_trans[3][4];
double patt_center[2] = { 0.0, 0.0 };
double patt_width = 80.0;
int thresh = 100;

void MainLoop(void);
void KeyEvent(unsigned char key, int x, int y);
void MouseEvent(int button, int state, int x, int y);
void Cleanup(void);
void DrawObject(void);

void mySetLight(void);
void mySetMaterial(void);


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

	if ((patt_id = arLoadPatt(patt_name)) < 0) {
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
		if (patt_id == marker_info[j].id) {
			if (k == -1) k = j;
			else if (marker_info[k].cf < marker_info[j].cf) k = j;

		}

	}

	if (k != -1) {
		arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
		DrawObject();
	}

	argSwapBuffers();
}


void DrawObject(void)
{
	double gl_para[16];

	argDrawMode3D();
	argDraw3dCamera(0, 0);
	
	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);
	
	//glTranslatef(0.0, 0.0, 20.0);
	//glColor3f(0.0, 1.0, 0.0);
//	glLineWidth(3.0);
//	glutWireCube(40.0);

	//glutWireTeapot(10);
	

	/*glBegin(GL_POLYGON);
		glVertex3f(-patt_width / 2.0, -patt_width / 2.0, 0.0);
		glVertex3f( patt_width / 2.0, -patt_width / 2.0, 0.0);
		glVertex3f( patt_width / 2.0,  patt_width / 2.0, 0.0);
		glVertex3f(-patt_width / 2.0,  patt_width / 2.0, 0.0);
	glEnd();
	*/

	
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
		glTranslatef(40.0, 0.0, 20.0);
		glColor3f(0.0, 1.0, 0.0);
		glutSolidSphere(20.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0, 0.0, 10.0);
		glColor3f(1.0, 0.0, 0.0);
		glutSolidCube(20.0);
	glPopMatrix();


	glDisable(GL_DEPTH_TEST);
	
}

void mySetLight(void) {
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 0.1 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };
	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
}

void mySetMaterial(void) {
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat shininess[] = { 50.0 };


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

