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

	if ((config = arMultiReadConfigFile(config_name)) == NULL) {
		printf("マルチマーカの設定ファイルの読み込みに失敗しました\n");
		return -1;
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0);

	arVideoCapStrt();

	argMainLoop(NULL, KeyEvent, MainLoop);

	return 0;
	


}