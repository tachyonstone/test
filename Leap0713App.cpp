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
		// ウィンドウの位置とサイズを設定
		setWindowPos(50, 50);
		setWindowSize(1280, 700);

		// 光源を追加する
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// 表示フォントの設定
		mFont = Font("YuGothicUI", 20);

		// カメラ(視点)の設定
		float y = 250;
		mCam.setEyePoint(Vec3f(0.0f, y, 500.0f));
		mCam.setCenterOfInterestPoint(Vec3f(0.0f, y, 0.0f));
		mCam.setPerspective(45.0f, getWindowAspectRatio(), 5.0f, 3000.0f);

		mMayaCam.setCurrentCam(mCam);

		// 描画時に奥行きの考慮を有効にする
		gl::enableDepthRead();

		// Leap Motion関連のセットアップ
		setupLeapObject();
	}

	// マウスダウン
	void mouseDown(MouseEvent event)
	{
		mMayaCam.mouseDown(event.getPos());
	}

	// マウスのドラッグ
	void mouseDrag(MouseEvent event)
	{
		mMayaCam.mouseDrag(event.getPos(), event.isLeftDown(),
			event.isMiddleDown(), event.isRightDown());
	}

	// 更新処理
	void update()
	{
		// フレームの更新
		mLastFrame = mCurrentFrame;
		mCurrentFrame = mLeap.frame();

		renderFrameParameter();
	}

	// 描画処理
	void draw()
	{
		gl::clear(Color(0, 0, 0));

		drawLeapObject();
		drawTexture();
	}

	// Leap Motion関連のセットアップ
	void setupLeapObject()
	{
	}

	// フレーム情報の描画
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


		// フレームレート
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
		// 指の座標を取得する
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

		// テキストボックスを作成する
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

	// Leap Motion関連の描画
	void drawLeapObject()
	{
		// 表示座標系の保持
		gl::pushMatrices();

		// カメラ位置を設定する
		gl::setMatrices(mMayaCam.getCamera());

		// 表示処理

		// 表示座標系を戻す
		gl::popMatrices();
	}

	// テクスチャの描画
	void drawTexture()
	{
		if (mTextTexture) {
			gl::draw(mTextTexture);
		}
	}

	// GL_LIGHT0の色を変える
	void setDiffuseColor(ci::ColorA diffuseColor)
	{
		gl::color(diffuseColor);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
	}

	// Leap SDKのVectorをCinderのVec3fに変換する
	Vec3f toVec3f(Leap::Vector vec)
	{
		return Vec3f(vec.x, vec.y, vec.z);
	}

	// カメラ
	CameraPersp  mCam;
	MayaCamUI    mMayaCam;

	// パラメータ表示用のテクスチャ
	gl::Texture mTextTexture;
	Font mFont;

	// Leap Motion
	Leap::Controller mLeap;
	Leap::Frame mCurrentFrame;
	Leap::Frame mLastFrame;
};

CINDER_APP_NATIVE(LeapApp, RendererGl)
