#include "common.h"
#include "virtualObjectSystem.h"
#include "recvSystem.h"
#include "bulletSystem.h"


// 剛体オブジェクト生成用
btRigidBody* body[2];
btSoftBody* sphere[2];

static int trans_obj = 0;
static float pos_x_tmp = 0;
static float pos_y_tmp = 0;
static int joint_status = 0;

const float model_x[] = { -1.0, 2.0 };
const float model_y[] = { 10.0, 10.0 };
const float model_z[] = { 2.0, 2.0 };

// ジェスチャによる仮想物体のサイズ指定
int create_stat = 0;
int dir_index = 0;
int dir_min = 500;
int create_num = 0;

extern btAlignedObjectArray<btCollisionShape*>	g_vCollisionShapes;	 //!< 剛体オブジェクトの形状を格納する動的配列

extern btSoftRigidDynamicsWorld* g_pDynamicsWorld;	//!< Bulletワールド
extern btSoftBodyWorldInfo g_softBodyWorldInfo;


//隠面処理 しきい値
float green_thr = 0;


/*!
* 操作対象の仮想物体の生成サイズ決定
*/
float DetermineModelSize(int spec_size) {

	if (spec_size <= 50) {
		return 0.4;
	}
	else if (50 < spec_size && spec_size <= 80) {
		return 0.6;
	}
	else if (80 < spec_size && spec_size <= 110) {
		return 0.8;
	}
	else {
		return 1.0;
	}

}


/*!
* 操作対象の仮想物体結合
*/
void AppendTransObj(btTransform trans) {
	bulletSystem bullet;

	const btScalar CUBE_HALF_EXTENTS = 0.2;	// 立方体の変の長さの半分(中心から辺までの距離)

	// 形状設定
	btCollisionShape *box_shape = new btBoxShape(btVector3(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS));

	// 移動用仮想物体追加
	body[create_num] = bullet.CreateRigidBody(0.0, trans, box_shape, 0);
	g_vCollisionShapes.push_back(box_shape); // 最後に破棄(delete)するために形状データを格納しておく
	body[create_num]->setGravity(btVector3(0, 0, 0));
	sphere[create_num]->appendAnchor(2, body[create_num]);

}


/*!
* 操作対象の仮想物体生成
*/
void virtualObjectSystem::CreateModel() {

	recvSystem recive;
	btTransform trans;	// 剛体オブジェクトの位置姿勢を格納する変数(行列)
	float create_r;

	recive.RecvHandData();

	if (create_stat == 2 && create_num < 2) {

		create_r = DetermineModelSize(dir_min);

		sphere[create_num] = btSoftBodyHelpers::CreateEllipsoid(g_softBodyWorldInfo,
			btVector3(model_x[create_num], model_y[create_num], model_z[create_num]),	// 中心座標
			btVector3(create_r, create_r, create_r), // 各軸方向の半径
			40  // 総ノード数
		);

		sphere[create_num]->getCollisionShape()->setMargin(0.5);
		sphere[create_num]->setTotalMass(10.0);
		sphere[create_num]->setTotalDensity(3.0);
		sphere[create_num]->m_materials[0]->m_kLST = 0.5;
		sphere[create_num]->m_cfg.kVC = 20.0;
		sphere[create_num]->setPose(true, true); //true, false

		btSoftBody::Material* pm = sphere[create_num]->appendMaterial();
		pm->m_kLST = 0.4;  //0.4
		pm->m_flags -= btSoftBody::fMaterial::DebugDraw;
		sphere[create_num]->m_cfg.kDF = 0;
		sphere[create_num]->m_cfg.kSRHR_CL = 1;
		sphere[create_num]->m_cfg.kSR_SPLT_CL = 0;
		sphere[create_num]->m_cfg.kDP = 1;  // 動かなくするため
		sphere[create_num]->m_cfg.collisions = btSoftBody::fCollision::CL_RS;
		sphere[create_num]->generateBendingConstraints(2, pm);

		///pass zero in generateClusters to create cluster for each tetrahedron or triangle
		sphere[create_num]->generateClusters(64);

		g_pDynamicsWorld->addSoftBody(sphere[create_num]);

		sphere[create_num]->setMass(27, 0.f);  // 固定
		sphere[create_num]->setMass(5, 0.f);  // 固定

		create_stat = 0;

		if (create_num == 0) {
			// 初期位置・姿勢
			btQuaternion qrot(0, 0, 0, 1);
			trans.setIdentity();// 位置姿勢行列の初期化
			trans.setOrigin(btVector3(model_x[0], model_y[0], model_z[0] - 1.0));
			trans.setRotation(qrot);	// 四元数を行列に変換して姿勢行列に掛け合わせる
		}
		else if (create_num == 1) {
			// 初期位置・姿勢
			btQuaternion qrot(0, 0, 0, 1);
			trans.setIdentity();// 位置姿勢行列の初期化
			trans.setOrigin(btVector3(model_x[1], model_y[1], model_z[1] - 1.0));
			trans.setRotation(qrot);	// 四元数を行列に変換して姿勢行列に掛け合わせる
		}

		AppendTransObj(trans);

		create_num++;
		dir_min = 500;  // 初期化
	}
}


/*!
* 剛体オブジェクトの追加
*/
void virtualObjectSystem::SetRigidBodies(void)
{
	bulletSystem bullet;

	btTransform trans;	// 剛体オブジェクトの位置姿勢を格納する変数(行列)

	trans.setIdentity();// 位置姿勢行列の初期化

	const btScalar CUBE_HALF_EXTENTS = 5.0;	// 立方体の変の長さの半分(中心から辺までの距離) 0.2
	const btScalar GROUND_HEIGHT = 0.0;		// 地面の高さ 0.0

	////////////////////////////////////////
	// 親指先の球
	btCollisionShape *ground_shape = new btSphereShape(1.2);	// 形状 

	trans.setOrigin(btVector3(0, 10, 0.5));
	
	// 剛体オブジェクト(Static)生成
	btRigidBody* body0 = bullet.CreateRigidBody(0.0, trans, ground_shape, 0);	// Body
	g_vCollisionShapes.push_back(ground_shape); // 最後に破棄(delete)するために形状データを格納しておく
	
	// すり抜け防止用Swept sphereの設定(CCD:Continuous Collision Detection)
	body0->setCcdMotionThreshold(CUBE_HALF_EXTENTS);
	body0->setCcdSweptSphereRadius(0.05*CUBE_HALF_EXTENTS);


	///////////////////////////////////////
	// 人差し指先の球
	btCollisionShape *ground_shape1 = new btSphereShape(0.8);	// 形状

	trans.setOrigin(btVector3(2, 10, 1));

	// 剛体オブジェクト(Static)生成
	btRigidBody* body1 = bullet.CreateRigidBody(0.0, trans, ground_shape1, 0);	// Body
	g_vCollisionShapes.push_back(ground_shape1); // 最後に破棄(delete)するために形状データを格納しておく

	// すり抜け防止用Swept sphereの設定(CCD:Continuous Collision Detection)
	body1->setCcdMotionThreshold(CUBE_HALF_EXTENTS);
	body1->setCcdSweptSphereRadius(0.05*CUBE_HALF_EXTENTS);

}


#if 0
/*!
* 親指の先の仮想物体の移動
*/
void TransByThumb(btRigidBody* body, btVector3 *pos) {
	if (*pos[0] > finger_x[0] - 1) {
		(*body).translate(btVector3(-0.5, 0, 0));
	}

	if (*pos[1] < finger_y[0]) {
		body->translate(btVector3(0, 0.5, 0));
	}

	if (*pos[0] < finger_x[0] - 1) {
		body->translate(btVector3(0.5, 0, 0));
	}

	if (*pos[1] > finger_y[0]) {
		body->translate(btVector3(0, -0.5, 0));
	}
}


/*!
* 人差し指の先の仮想物体の移動
*/
void TransByIndex(btRigidBody* body, btVector3 *pos) {
	if (*pos[0] > finger_x[1]) {
		body->translate(btVector3(-0.6, 0, 0));
	}

	if (*pos[1] < finger_y[1]) {
		body->translate(btVector3(0, 0.6, 0));
	}

	if (*pos[0] < finger_x[1]) {
		body->translate(btVector3(0.5, 0, 0));
	}

	if (*pos[1] > finger_y[1] / 50 + 10.0) {
		body->translate(btVector3(0, -0.5, 0));
	}
}

#endif


/*!
* Bulletのオブジェクトシーン描画
*/
void virtualObjectSystem::DrawBulletObjects(void)
{
	virtualObjectSystem virtualObject;
	bulletSystem bullet;

	/*
	static const GLfloat difr[] = { 1.0, 0.4, 0.4, 1.0 };	// 拡散色 : 赤
	static const GLfloat difg[] = { 0.4, 0.6, 0.4, 1.0 };	// 拡散色 : 緑
	static const GLfloat difb[] = { 0.4, 0.4, 0.8, 1.0 };	// 拡散色 : 青
	static const GLfloat spec[] = { 0.3, 0.3, 0.3, 1.0 };	// 鏡面反射色
	static const GLfloat ambi[] = { 0.1, 0.1, 0.5, 1.0 };	// 環境光
	*/

	static const GLfloat difr[] = { 1.0, 0.4, 0.4, 1.0 };	// 拡散色 : 赤
	static const GLfloat difg[] = { 0.4, 0.6, 0.4, 1.0 };	// 拡散色 : 緑
	static const GLfloat difb[] = { 0.4, 0.4, 1.0, 1.0 };	// 拡散色 : 青
	static const GLfloat spec[] = { 0.3, 0.3, 0.3, 1.0 };	// 鏡面反射色
	static const GLfloat ambi[] = { 0.1, 0.1, 0.1, 1.0 };	// 環境光

	virtualObject.CreateModel();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDisable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambi);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.0, 0.0, 1.0);

	glDisable(GL_CULL_FACE);

	btVector3	world_min, world_max;
	g_pDynamicsWorld->getBroadphase()->getBroadphaseAabb(world_min, world_max);

	if (g_pDynamicsWorld) {
		btScalar m[16];
		btMatrix3x3	rot;
		rot.setIdentity();

		// Bulletワールドから剛体オブジェクト情報を取得してOpenGLで描画
		int n = g_pDynamicsWorld->getNumCollisionObjects();	// オブジェクト数の取得
		for (int i = 0; i < n; ++i) {

			// btCollisionObjectを取得
			btCollisionObject* obj = g_pDynamicsWorld->getCollisionObjectArray()[i];

			// 形状取得
			btCollisionShape* shape = obj->getCollisionShape();
			int shapetype = shape->getShapeType();

			if (shapetype == SOFTBODY_SHAPE_PROXYTYPE) {
				// btSoftBodyへのキャスト
				btSoftBody* body = btSoftBody::upcast(obj);

				// 材質設定
				glMaterialfv(GL_FRONT, GL_DIFFUSE, difb);

				// SoftBodyの描画
				bullet.DrawBulletSoftBody(body);

			}
			else {
				// btRigidBodyへのキャスト
				btRigidBody* body = btRigidBody::upcast(obj);

				btVector3 pos = body->getCenterOfMassPosition();

				if (body && body->getMotionState()) {
					// btRigidBodyからMotion Stateを取得して，OpenGLの変換行列として位置・姿勢情報を得る
					btDefaultMotionState* ms = (btDefaultMotionState*)body->getMotionState();
					ms->m_graphicsWorldTrans.getOpenGLMatrix(m);
					rot = ms->m_graphicsWorldTrans.getBasis();
				}
				else {
					obj->getWorldTransform().getOpenGLMatrix(m);
					rot = obj->getWorldTransform().getBasis();
				}

				if (body && body->getInvMass() > RX_FEQ_EPS) { // 通常の物体は青で描画
					glMaterialfv(GL_FRONT, GL_DIFFUSE, difb);
				}
				else {	// Kinematicボディの場合は緑で描画

					if (i == 1) {
						glMaterialfv(GL_FRONT, GL_DIFFUSE, difg);
					}
					else {
						glMaterialfv(GL_FRONT, GL_DIFFUSE, difr);
					}

				}

				glPushMatrix();
				glMultMatrixf(m);

				pos = body->getCenterOfMassPosition(); 
				glTranslatef(pos[0], pos[1], pos[2]);

				// 形状の種類ごとに描画
				if (shapetype == BOX_SHAPE_PROXYTYPE) {
					// ボックス形状
					const btBoxShape* box = static_cast<const btBoxShape*>(shape);
					btVector3 half_extent = box->getHalfExtentsWithMargin();
					glScaled(2 * half_extent[0], 2 * half_extent[1], 2 * half_extent[2]);
					glutSolidCube(1.0);  //色がつく,これをコメントアウトすると物体が透明になる
				}
				else if (shapetype == SPHERE_SHAPE_PROXYTYPE) {
					// 球形状
					const btSphereShape* sphere = static_cast<const btSphereShape*>(shape);
					double rad = sphere->getRadius();
					glutSolidSphere(rad, 32, 32);
				}
				else if (shapetype == CYLINDER_SHAPE_PROXYTYPE) {
					// 円筒形状
					const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(shape);
					double rad = cylinder->getRadius();
					double len = cylinder->getHalfExtentsWithMargin()[cylinder->getUpAxis()] * 2;
					DrawCylinder(rad, len, cylinder->getUpAxis(), 32);
				}
				else {
					glutSolidSphere(1.0, 10, 10);
				}

				glTranslatef(pos[0], pos[1], pos[2]);

				// Thumb
				if (i == 0) {
					//TransByThumb(body, &pos);

					if (pos[0] > finger_x[0] - 1) {
						body->translate(btVector3(-0.5, 0, 0));
					}

					if (pos[1] < finger_y[0]) {
						body->translate(btVector3(0, 0.5, 0));
					}

					if (pos[0] < finger_x[0] - 1) {
						body->translate(btVector3(0.5, 0, 0));
					}

					if (pos[1] > finger_y[0]) {
						body->translate(btVector3(0, -0.5, 0));
					}
					

				}

				// Index finger
				if (i == 1) {
					//TransByIndex(body, &pos);

					if (pos[0] > finger_x[1]) {
						body->translate(btVector3(-0.6, 0, 0));
					}

					if (pos[1] < finger_y[1]) {
						body->translate(btVector3(0, 0.6, 0));
					}

					if (pos[0] < finger_x[1]) {
						body->translate(btVector3(0.5, 0, 0));
					}

					if (pos[1] > finger_y[1] / 50 + 10.0) {
						body->translate(btVector3(0, -0.5, 0));
					}

				}

				/* オブジェクトの移動 */
				if (n > 3 && i == 3) { // モデルが2つ以上のとき
					pos_x_tmp = pos[0];
					pos_y_tmp = pos[1];
				}

				//test
				trans_obj = 1;

				// 移動用仮想物体の場所により，隠面処理する
				if (i == 5) {
					green_thr = pos[1] + 10;

				}

				if ((i == 5 && trans_obj == 1) && 
					(finger_x[1] > finger_x[2]) && 
					(finger_x[2] > finger_x[3]) &&
					(finger_x[3] > finger_x[4])
					) {   //手のひらがうえ
					sphere[trans_obj]->setMass(27, 0.9f);  // 固定解除
					sphere[trans_obj]->setMass(5, 0.9f);  // 固定解除

					if (pos[0] > finger_x[3]) {
						body->translate(btVector3(-0.6 - joint_status, 0, 0));

					}
					if (pos[1] < finger_y[3]) {
						body->translate(btVector3(0, 0.6, 0));
					}
					if (pos[0] < finger_x[3] + joint_status * 30) {
						body->translate(btVector3(0.6 + joint_status, 0, 0));

					}
					if (pos[1] > finger_y[3]) {
						body->translate(btVector3(0, -0.6, 0));
					}

					if (joint_status == 0 && n > 3 &&
						(pos[0] < pos_x_tmp && pos[0] > pos_x_tmp - 2.0) &&
						(pos[1] < pos_y_tmp + 1.0 && pos[1] > pos_y_tmp - 2.0)) {

						sphere[0]->setMass(27, 0.9f);  // 固定解除
						sphere[0]->setMass(5, 0.9f);  // 固定解除

						joint_status++;  // 結合済みにする
						btSoftBody::LJoint::Specs	lj;
						btSoftBody::AJoint::Specs   aj;

						lj.position = btVector3(pos[0] - 0.5, pos[1] - 2.0, model_z[0]);
						aj.axis = btVector3(0, 0, 0);
						sphere[0]->appendAngularJoint(aj, sphere[1]);
						sphere[0]->appendLinearJoint(lj, sphere[1]);
						
					} // 結合

				}
				else if ((i == 5 && trans_obj == 1) &&
					(finger_x[1] < finger_x[2]) &&
					(finger_x[2] < finger_x[3]) &&
					(finger_x[3] < finger_x[4])
					) {

					sphere[trans_obj]->setMass(27, 0.0f);  // 固定
					sphere[0]->setMass(5, 0.0f);  // 固定解除

				}
				/* ここまでオブジェクトの移動 */

				//結合後の移動
				if ((i == 3 && joint_status == 1) && 
					(finger_x[1] > finger_x[2]) && 
					(finger_x[2] > finger_x[3]) &&
					(finger_x[3] < finger_x[4])
					) {   //手のひらがうえ

					if (pos[0] > finger_x[3]) {
						body->translate(btVector3(-1.6, 0, 0));
					}
					if (pos[1] < finger_y[3]) {
						body->translate(btVector3(0, 0.6, 0));
					}
					if (pos[0] < finger_x[3] + 30) {
						body->translate(btVector3(1.6, 0, 0));

					}
					if (pos[1] > finger_y[3]) {
						body->translate(btVector3(0, -0.6, 0));
					}

				}
				else if (joint_status == 1 &&
					(i == 3 && trans_obj == 1) &&
					(finger_x[1] < finger_x[2]) &&
					(finger_x[2] < finger_x[3]) &&
					(finger_x[3] < finger_x[4])
					) {

					sphere[trans_obj]->setMass(27, 0.0f);
				}

				glPopMatrix();
			}

		} // End Of for (draw OpenGL)
	}

}
