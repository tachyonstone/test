#include "common.h"
#include "virtualObjectSystem.h"
#include "recvSystem.h"
#include "bulletSystem.h"


// ���̃I�u�W�F�N�g�����p
btRigidBody* body[2];
btSoftBody* sphere[2];

static int trans_obj = 0;
static float pos_x_tmp = 0;
static float pos_y_tmp = 0;
static int joint_status = 0;

const float model_x[] = { -1.0, 2.0 };
const float model_y[] = { 10.0, 10.0 };
const float model_z[] = { 2.0, 2.0 };

// �W�F�X�`���ɂ�鉼�z���̂̃T�C�Y�w��
int create_stat = 0;
int dir_index = 0;
int dir_min = 500;
int create_num = 0;

extern btAlignedObjectArray<btCollisionShape*>	g_vCollisionShapes;	 //!< ���̃I�u�W�F�N�g�̌`����i�[���铮�I�z��

extern btSoftRigidDynamicsWorld* g_pDynamicsWorld;	//!< Bullet���[���h
extern btSoftBodyWorldInfo g_softBodyWorldInfo;


//�B�ʏ��� �������l
float green_thr = 0;


/*!
* ����Ώۂ̉��z���̂̐����T�C�Y����
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
* ����Ώۂ̉��z���̌���
*/
void AppendTransObj(btTransform trans) {
	bulletSystem bullet;

	const btScalar CUBE_HALF_EXTENTS = 0.2;	// �����̂̕ς̒����̔���(���S����ӂ܂ł̋���)

	// �`��ݒ�
	btCollisionShape *box_shape = new btBoxShape(btVector3(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS));

	// �ړ��p���z���̒ǉ�
	body[create_num] = bullet.CreateRigidBody(0.0, trans, box_shape, 0);
	g_vCollisionShapes.push_back(box_shape); // �Ō�ɔj��(delete)���邽�߂Ɍ`��f�[�^���i�[���Ă���
	body[create_num]->setGravity(btVector3(0, 0, 0));
	sphere[create_num]->appendAnchor(2, body[create_num]);

}


/*!
* ����Ώۂ̉��z���̐���
*/
void virtualObjectSystem::CreateModel() {

	recvSystem recive;
	btTransform trans;	// ���̃I�u�W�F�N�g�̈ʒu�p�����i�[����ϐ�(�s��)
	float create_r;

	recive.RecvHandData();

	if (create_stat == 2 && create_num < 2) {

		create_r = DetermineModelSize(dir_min);

		sphere[create_num] = btSoftBodyHelpers::CreateEllipsoid(g_softBodyWorldInfo,
			btVector3(model_x[create_num], model_y[create_num], model_z[create_num]),	// ���S���W
			btVector3(create_r, create_r, create_r), // �e�������̔��a
			40  // ���m�[�h��
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
		sphere[create_num]->m_cfg.kDP = 1;  // �����Ȃ����邽��
		sphere[create_num]->m_cfg.collisions = btSoftBody::fCollision::CL_RS;
		sphere[create_num]->generateBendingConstraints(2, pm);

		///pass zero in generateClusters to create cluster for each tetrahedron or triangle
		sphere[create_num]->generateClusters(64);

		g_pDynamicsWorld->addSoftBody(sphere[create_num]);

		sphere[create_num]->setMass(27, 0.f);  // �Œ�
		sphere[create_num]->setMass(5, 0.f);  // �Œ�

		create_stat = 0;

		if (create_num == 0) {
			// �����ʒu�E�p��
			btQuaternion qrot(0, 0, 0, 1);
			trans.setIdentity();// �ʒu�p���s��̏�����
			trans.setOrigin(btVector3(model_x[0], model_y[0], model_z[0] - 1.0));
			trans.setRotation(qrot);	// �l�������s��ɕϊ����Ďp���s��Ɋ|�����킹��
		}
		else if (create_num == 1) {
			// �����ʒu�E�p��
			btQuaternion qrot(0, 0, 0, 1);
			trans.setIdentity();// �ʒu�p���s��̏�����
			trans.setOrigin(btVector3(model_x[1], model_y[1], model_z[1] - 1.0));
			trans.setRotation(qrot);	// �l�������s��ɕϊ����Ďp���s��Ɋ|�����킹��
		}

		AppendTransObj(trans);

		create_num++;
		dir_min = 500;  // ������
	}
}


/*!
* ���̃I�u�W�F�N�g�̒ǉ�
*/
void virtualObjectSystem::SetRigidBodies(void)
{
	bulletSystem bullet;

	btTransform trans;	// ���̃I�u�W�F�N�g�̈ʒu�p�����i�[����ϐ�(�s��)

	trans.setIdentity();// �ʒu�p���s��̏�����

	const btScalar CUBE_HALF_EXTENTS = 5.0;	// �����̂̕ς̒����̔���(���S����ӂ܂ł̋���) 0.2
	const btScalar GROUND_HEIGHT = 0.0;		// �n�ʂ̍��� 0.0

	////////////////////////////////////////
	// �e�w��̋�
	btCollisionShape *ground_shape = new btSphereShape(1.2);	// �`�� 

	trans.setOrigin(btVector3(0, 10, 0.5));
	
	// ���̃I�u�W�F�N�g(Static)����
	btRigidBody* body0 = bullet.CreateRigidBody(0.0, trans, ground_shape, 0);	// Body
	g_vCollisionShapes.push_back(ground_shape); // �Ō�ɔj��(delete)���邽�߂Ɍ`��f�[�^���i�[���Ă���
	
	// ���蔲���h�~�pSwept sphere�̐ݒ�(CCD:Continuous Collision Detection)
	body0->setCcdMotionThreshold(CUBE_HALF_EXTENTS);
	body0->setCcdSweptSphereRadius(0.05*CUBE_HALF_EXTENTS);


	///////////////////////////////////////
	// �l�����w��̋�
	btCollisionShape *ground_shape1 = new btSphereShape(0.8);	// �`��

	trans.setOrigin(btVector3(2, 10, 1));

	// ���̃I�u�W�F�N�g(Static)����
	btRigidBody* body1 = bullet.CreateRigidBody(0.0, trans, ground_shape1, 0);	// Body
	g_vCollisionShapes.push_back(ground_shape1); // �Ō�ɔj��(delete)���邽�߂Ɍ`��f�[�^���i�[���Ă���

	// ���蔲���h�~�pSwept sphere�̐ݒ�(CCD:Continuous Collision Detection)
	body1->setCcdMotionThreshold(CUBE_HALF_EXTENTS);
	body1->setCcdSweptSphereRadius(0.05*CUBE_HALF_EXTENTS);

}


#if 0
/*!
* �e�w�̐�̉��z���̂̈ړ�
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
* �l�����w�̐�̉��z���̂̈ړ�
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
* Bullet�̃I�u�W�F�N�g�V�[���`��
*/
void virtualObjectSystem::DrawBulletObjects(void)
{
	virtualObjectSystem virtualObject;
	bulletSystem bullet;

	/*
	static const GLfloat difr[] = { 1.0, 0.4, 0.4, 1.0 };	// �g�U�F : ��
	static const GLfloat difg[] = { 0.4, 0.6, 0.4, 1.0 };	// �g�U�F : ��
	static const GLfloat difb[] = { 0.4, 0.4, 0.8, 1.0 };	// �g�U�F : ��
	static const GLfloat spec[] = { 0.3, 0.3, 0.3, 1.0 };	// ���ʔ��ːF
	static const GLfloat ambi[] = { 0.1, 0.1, 0.5, 1.0 };	// ����
	*/

	static const GLfloat difr[] = { 1.0, 0.4, 0.4, 1.0 };	// �g�U�F : ��
	static const GLfloat difg[] = { 0.4, 0.6, 0.4, 1.0 };	// �g�U�F : ��
	static const GLfloat difb[] = { 0.4, 0.4, 1.0, 1.0 };	// �g�U�F : ��
	static const GLfloat spec[] = { 0.3, 0.3, 0.3, 1.0 };	// ���ʔ��ːF
	static const GLfloat ambi[] = { 0.1, 0.1, 0.1, 1.0 };	// ����

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

		// Bullet���[���h���獄�̃I�u�W�F�N�g�����擾����OpenGL�ŕ`��
		int n = g_pDynamicsWorld->getNumCollisionObjects();	// �I�u�W�F�N�g���̎擾
		for (int i = 0; i < n; ++i) {

			// btCollisionObject���擾
			btCollisionObject* obj = g_pDynamicsWorld->getCollisionObjectArray()[i];

			// �`��擾
			btCollisionShape* shape = obj->getCollisionShape();
			int shapetype = shape->getShapeType();

			if (shapetype == SOFTBODY_SHAPE_PROXYTYPE) {
				// btSoftBody�ւ̃L���X�g
				btSoftBody* body = btSoftBody::upcast(obj);

				// �ގ��ݒ�
				glMaterialfv(GL_FRONT, GL_DIFFUSE, difb);

				// SoftBody�̕`��
				bullet.DrawBulletSoftBody(body);

			}
			else {
				// btRigidBody�ւ̃L���X�g
				btRigidBody* body = btRigidBody::upcast(obj);

				btVector3 pos = body->getCenterOfMassPosition();

				if (body && body->getMotionState()) {
					// btRigidBody����Motion State���擾���āCOpenGL�̕ϊ��s��Ƃ��Ĉʒu�E�p�����𓾂�
					btDefaultMotionState* ms = (btDefaultMotionState*)body->getMotionState();
					ms->m_graphicsWorldTrans.getOpenGLMatrix(m);
					rot = ms->m_graphicsWorldTrans.getBasis();
				}
				else {
					obj->getWorldTransform().getOpenGLMatrix(m);
					rot = obj->getWorldTransform().getBasis();
				}

				if (body && body->getInvMass() > RX_FEQ_EPS) { // �ʏ�̕��̂͐ŕ`��
					glMaterialfv(GL_FRONT, GL_DIFFUSE, difb);
				}
				else {	// Kinematic�{�f�B�̏ꍇ�͗΂ŕ`��

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

				// �`��̎�ނ��Ƃɕ`��
				if (shapetype == BOX_SHAPE_PROXYTYPE) {
					// �{�b�N�X�`��
					const btBoxShape* box = static_cast<const btBoxShape*>(shape);
					btVector3 half_extent = box->getHalfExtentsWithMargin();
					glScaled(2 * half_extent[0], 2 * half_extent[1], 2 * half_extent[2]);
					glutSolidCube(1.0);  //�F����,������R�����g�A�E�g����ƕ��̂������ɂȂ�
				}
				else if (shapetype == SPHERE_SHAPE_PROXYTYPE) {
					// ���`��
					const btSphereShape* sphere = static_cast<const btSphereShape*>(shape);
					double rad = sphere->getRadius();
					glutSolidSphere(rad, 32, 32);
				}
				else if (shapetype == CYLINDER_SHAPE_PROXYTYPE) {
					// �~���`��
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

				/* �I�u�W�F�N�g�̈ړ� */
				if (n > 3 && i == 3) { // ���f����2�ȏ�̂Ƃ�
					pos_x_tmp = pos[0];
					pos_y_tmp = pos[1];
				}

				//test
				trans_obj = 1;

				// �ړ��p���z���̂̏ꏊ�ɂ��C�B�ʏ�������
				if (i == 5) {
					green_thr = pos[1] + 10;

				}

				if ((i == 5 && trans_obj == 1) && 
					(finger_x[1] > finger_x[2]) && 
					(finger_x[2] > finger_x[3]) &&
					(finger_x[3] > finger_x[4])
					) {   //��̂Ђ炪����
					sphere[trans_obj]->setMass(27, 0.9f);  // �Œ����
					sphere[trans_obj]->setMass(5, 0.9f);  // �Œ����

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

						sphere[0]->setMass(27, 0.9f);  // �Œ����
						sphere[0]->setMass(5, 0.9f);  // �Œ����

						joint_status++;  // �����ς݂ɂ���
						btSoftBody::LJoint::Specs	lj;
						btSoftBody::AJoint::Specs   aj;

						lj.position = btVector3(pos[0] - 0.5, pos[1] - 2.0, model_z[0]);
						aj.axis = btVector3(0, 0, 0);
						sphere[0]->appendAngularJoint(aj, sphere[1]);
						sphere[0]->appendLinearJoint(lj, sphere[1]);
						
					} // ����

				}
				else if ((i == 5 && trans_obj == 1) &&
					(finger_x[1] < finger_x[2]) &&
					(finger_x[2] < finger_x[3]) &&
					(finger_x[3] < finger_x[4])
					) {

					sphere[trans_obj]->setMass(27, 0.0f);  // �Œ�
					sphere[0]->setMass(5, 0.0f);  // �Œ����

				}
				/* �����܂ŃI�u�W�F�N�g�̈ړ� */

				//������̈ړ�
				if ((i == 3 && joint_status == 1) && 
					(finger_x[1] > finger_x[2]) && 
					(finger_x[2] > finger_x[3]) &&
					(finger_x[3] < finger_x[4])
					) {   //��̂Ђ炪����

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
