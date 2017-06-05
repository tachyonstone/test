#include <windows.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <arMulti.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>


char *vconf_name = "Data/WDM_camera_flipV.xml";
char *cparam_name = "Data/camera_para.dat";
char *config_name = "Data/multi/marker.dat";

ARMultiMarkerInfoT *config;
int thresh = 100;

void MainLoop(void);
void mySetLight(void);
void mySetMaterial(void);
void DrawObject(double trans_cr[3][4], double trans_rm[3][4], int visible);
void KeyEvent(unsigned char key, int x, int y);
void Cleanup(void);

int main(int argc, char **argv)
{
	ARParam cparam;
	ARParam wparam;
	int xsize, ysize;

	glutInit(&argc, argv);

	if (arVideoOpen(vconf_name) < 0) {
		printf("�r�f�I�f�o�C�X�̃G���[");
		return -1;

	}

	if (arVideoInqSize(&xsize, &ysize) < 0)	return -1;

	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("�J�����p�����[�^�̓ǂݍ��݂Ɏ��s���܂���\n");
		return -1;

	}

	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	if ((config = arMultiReadConfigFile(config_name)) == NULL) {
		printf("�}���`�}�[�J�̐ݒ�t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
		return -1;
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0);

	arVideoCapStrt();

	argMainLoop(NULL, KeyEvent, MainLoop);

	return 0;

}

void MainLoop(void)
{
	ARUnit8 *image;
	ARMarkerInfo *marker_info;
	int marker_num;
	int i;

	if ((image = arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}

	argDrawMode2D();
	argDispImage(image, 0, 0);

	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleaneup();
		exit(0);
	}

	arVideoCapNext();

	if (arMultiGetTransMat(marker_info, marker_num, config) >= 0) {
		argDrawModel3D();
		argDraw3dCamera(0, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		for (i = 0; i < config->marker_num; i++) {
			DrawObject(config->trans, config->marker[i].trans, config->marker[i].visible);
		}

		glDisable(GL_DEPTH_TEST);

	}

	argSwapBuffers();

}


void mySetLight(void) {
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 0.1 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };
	GLfloat light_position[] = { 200.0, 200.0, 200.0, 0.0 };

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

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

}

void DrawObject(double trans_cr[3][4], double trans_rm[3][4], int visible)
{
	double Tcr[16];
	double Trm[16];

	argConvGlpara(trans_cr, Tcr);
	argConvGlpara(trans_rm, Trm);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(Tcr);

	glMultMatrixd(Trm);

	glTranslatef(0.0, 0.0, 23.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	if (visible >= 0) {
		mySetLight();
		glEnable(GL_LIGHTING);
		mySetMaterial();
		glutSolidTeapot(30.0);
		glDisable(GL_LIGHTING);
	}
	else {
		glColor3f(0.0, 0.0, 1.0);
		glutWireTeapot(30.0);

	}

}

void KeyEvent(unsigned char key, int x, int y)
{
	if (key == 0x1b) {
		Cleanup();
		exit(0);

	}
}

void Cleanup(void)
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();

	arMultiFreeConfig(config);

}
