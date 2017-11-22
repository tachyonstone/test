#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/MayaCamUI.h"

#include "Leap.h"

#include <WinSock2.h>
#include <stdio.h>
#include <string.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class LeapApp : public AppNative {
public:

	int finger_x;
	int finger_y;
	int finger_z;


	void setup()
	{
		// �E�B���h�E�̈ʒu�ƃT�C�Y��ݒ�
		setWindowPos(50, 50);
		setWindowSize(1280, 700);

		// ������ǉ�����
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// �\���t�H���g�̐ݒ�
		mFont = Font("YuGothicUI", 20);

		// �J����(���_)�̐ݒ�
		float y = 250;
		mCam.setEyePoint(Vec3f(0.0f, y, 500.0f));
		mCam.setCenterOfInterestPoint(Vec3f(0.0f, y, 0.0f));
		mCam.setPerspective(45.0f, getWindowAspectRatio(), 5.0f, 3000.0f);

		mMayaCam.setCurrentCam(mCam);

		// �`�掞�ɉ��s���̍l����L���ɂ���
		gl::enableDepthRead();

		// Leap Motion�֘A�̃Z�b�g�A�b�v
		setupLeapObject();
	}

	// �}�E�X�_�E��
	void mouseDown(MouseEvent event)
	{
		mMayaCam.mouseDown(event.getPos());
	}

	// �}�E�X�̃h���b�O
	void mouseDrag(MouseEvent event)
	{
		mMayaCam.mouseDrag(event.getPos(), event.isLeftDown(),
			event.isMiddleDown(), event.isRightDown());
	}

	// �X�V����
	void update()
	{
		// �t���[���̍X�V
		mLastFrame = mCurrentFrame;
		mCurrentFrame = mLeap.frame();

		renderFrameParameter();
	}

	// �`�揈��
	void draw()
	{
		gl::clear(Color(0, 0, 0));

		drawLeapObject();
		drawTexture();
	}

	// Leap Motion�֘A�̃Z�b�g�A�b�v
	void setupLeapObject()
	{
	}

	// �t���[�����̕`��
	void renderFrameParameter()
	{
		stringstream ss;
		char sendBuf[2048];
		stringstream kk;


		WSAData wsaData;

		SOCKET sock;
		struct sockaddr_in addr;

		WSAStartup(MAKEWORD(2, 0), &wsaData);

		sock = socket(AF_INET, SOCK_DGRAM, 0);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(12345);
		addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


		// �t���[�����[�g
		ss << "FPS : " << mCurrentFrame.currentFramesPerSecond() << "\n";

		ss << "Finger Count : " << mCurrentFrame.fingers().count() << "\n";


#if 0
		for (auto finger : mCurrentFrame.fingers().extended().fingerType(Leap::Finger::Type::TYPE_INDEX)) {
			ss << "Finger Position: " 
				<< finger.tipPosition().x << ", "
				<< finger.tipPosition().y << ", "
				<< finger.tipPosition().z << "\n";

			/*sprintf(sendBuf, "Finger Position : %d, \n %d, \n %d, \n",
				(int)finger.tipPosition().x,
				(int)finger.tipPosition().y,
				(int)finger.tipPosition().z);*/

			finger_x = (int)finger.tipPosition().x;
			finger_y = (int)finger.tipPosition().y;
			finger_z = (int)finger.tipPosition().z;

			sprintf(sendBuf, "%d %d %d", finger_x, finger_y, finger_z);

			sendto(sock, sendBuf, sizeof(sendBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
				
		}
#endif

#if 1
		int finger_no = 0;
		int count = 0;
		kk << "nasi";
		// �w�̍��W���擾����
		for (auto finger : mCurrentFrame.fingers()) {
			ss << "Finger Position: " << finger.tipPosition().x << ", "
				<< finger.tipPosition().y << ", "
				<< finger.tipPosition().z << "no:" << finger_no << "\n";

		
			/*sprintf(sendBuf, "Finger Position : %d, \n %d, \n %d, \n",
			(int)finger.tipPosition().x,
			(int)finger.tipPosition().y,
			(int)finger.tipPosition().z);*/

			finger_x = (int)finger.tipPosition().x;
			finger_y = (int)finger.tipPosition().y;
			finger_z = (int)finger.tipPosition().z;

			count += sprintf(sendBuf + count, "%d %d %d %d ", finger_no, finger_x, finger_y, finger_z);

			finger_no++;
		
		}

		if (count > 0) {
			printf("buf : %s\n", sendBuf);
			sendto(sock, sendBuf, sizeof(sendBuf), 0, (struct sockaddr *)&addr, sizeof(addr));
			//kk << sendBuf;
		}
#endif

		// �e�L�X�g�{�b�N�X���쐬����
		auto tbox = TextBox()
			.alignment(TextBox::LEFT)
			.font(mFont)
			.text(ss.str())
			.color(Color(1.0f, 1.0f, 1.0f))
			.backgroundColor(ColorA(0, 0, 0, 0.5f));

		mTextTexture = gl::Texture(tbox.render());

		closesocket(sock);
		WSACleanup();


	}

	// Leap Motion�֘A�̕`��
	void drawLeapObject()
	{
		// �\�����W�n�̕ێ�
		gl::pushMatrices();

		// �J�����ʒu��ݒ肷��
		gl::setMatrices(mMayaCam.getCamera());

		// �\������

		// �\�����W�n��߂�
		gl::popMatrices();
	}

	// �e�N�X�`���̕`��
	void drawTexture()
	{
		if (mTextTexture) {
			gl::draw(mTextTexture);
		}
	}

	// GL_LIGHT0�̐F��ς���
	void setDiffuseColor(ci::ColorA diffuseColor)
	{
		gl::color(diffuseColor);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
	}

	// Leap SDK��Vector��Cinder��Vec3f�ɕϊ�����
	Vec3f toVec3f(Leap::Vector vec)
	{
		return Vec3f(vec.x, vec.y, vec.z);
	}

	// �J����
	CameraPersp  mCam;
	MayaCamUI    mMayaCam;

	// �p�����[�^�\���p�̃e�N�X�`��
	gl::Texture mTextTexture;
	Font mFont;

	// Leap Motion
	Leap::Controller mLeap;
	Leap::Frame mCurrentFrame;
	Leap::Frame mLastFrame;
};

CINDER_APP_NATIVE(LeapApp, RendererGl)
