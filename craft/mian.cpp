/**
* @file main.c
* @author zf
* @date 2017/11/26
* @detail 尝试利用opengl去复刻minecraft
* @TODO 碰撞检测，地形生成，能加载minecraft的材质包，解决鼠标漂移问题，破坏方块，方块状态的记录，
*/
#define  _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<map>
#include<string>
#include"Crawler.h"
#include "visualBall.h"
#include "CubeManager.h"
#include "LightMaterial.h"
#include"Man.h"
#include"Chunk.h"

using namespace std;
////参数
#define DELAY 12
#define NUM_OF_LINE 32
#define BLOCK_SIZE 1
#define VIEW_SCALE 2
#define PI 3.1415926535898
#define MAX_CHAR 128


//相对坐标常量
//const double INCREMENT = 0.05;

/// 世界参数
Point center = Point(0, 0, 0);
Point lookAt = Point(5, 5, 5);
Point cameraAt = Point(5, 5, 5);
Point godAt = Point(8, 11, -18);

float last_mm_x = 0;
float last_mm_y = 0;

bool reset_god = false;
int view_person = 0;  // 0 上帝视角，1 第一人称，2 第三人称, 虚拟球
int last_view = 0;
enum {
	GOD, FIRST_PERSON, THIRD_PERSON, BALL
};
enum {
	HEAD, HAIR, BODY, THIGH_L, THIGH_R, ARM_L, ARMR, EYE_L, EYE_R, MOUTH
};

float near_sight = 0.1f;
float far_sight = 100;
int scr_w;
int scr_h;

/// 各种flag
extern bool trackingMouse;
extern bool redrawContinue;
extern bool trackballMove;
extern int scr_w;
extern int scr_h;
extern float curPos[3], dx, dy, dz;
extern float look_dy;
extern int curx, cury;
extern float angle, axis[3], trans[3];

/// 飞机模型参数,测试用
static int xRot = 0.;
static int yRot = 0;
static int zRot = 0;
static double x_air, y_air, z_air;
static double fly_distance = 20;

static float fly_speed = 0.15;
bool flying = false;
bool spining = false;
///



vector<Point> torch_arr;

// 跳跃参数
bool jumping = false;
bool falling = false;
float jump_speed = 0.3;
float fall_speed = 0.3;
static float max_height = 3.0;
static float ground_height = 1.00;


//相对坐标参数
bool left_forward = true;
bool right_forward = false;
bool look_from_left = true;


//光源太阳
GLfloat s_angle = 0.0f;
GLfloat LightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };        //光源位置

GLfloat ambient = 0.2f;
GLfloat LightAmbient[] = { ambient, ambient, ambient, 1.0f };        //环境光
GLfloat diffuse = 0.2f;
GLfloat LightDiffuse[] = { diffuse, 0, 0, 1.0f };        //漫反射
GLfloat specular = 0.2f;
GLfloat LightSpecular[] = { specular, 0.6f, 0.6f, 1.0f };    //镜面反射

															 //材料参数
GLfloat matEmissionR, matEmissionG, matEmissionB;
GLfloat MatEmission[] = { matEmissionR, matEmissionG, matEmissionB, 1.0f };
GLfloat matAmbientR, matAmbientG, matAmbientB;
GLfloat MatAmbient[] = { matAmbientR, matAmbientG, matAmbientB, 1.0f };
GLfloat matDiffuseR, matDiffuseG, matDiffuseB;
GLfloat MatDiffuse[] = { matDiffuseR, matDiffuseG, matDiffuseB, 1.0f };
GLfloat matSpecularR, matSpecularG, matSpecularB;
GLfloat MatSpecular[] = { matSpecularR, matSpecularG, matSpecularB, 1.0f };
GLfloat MatShininess;
//窗口大小
int WindowWidth = 1200;
int WindowHeight = 800;
CubeManager cube_mgr = CubeManager();
LightMaterial lightMaterial;
vector<Crawler> crawler_arr;
// 人物
Man man;
////////


int changePos(float pos) 
{
	if (pos < 0) 
	{
		return pos - 0.5;
	}
	else
	{
		return pos + 0.5;
	}
}

void UnProject(float mouse_x, float mouse_y, int c) 
{
	int x = mouse_x;                /* 屏幕坐标 */
	int y = mouse_y;
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLfloat winx, winy, winz;
	GLdouble posx, posy, posz;

	glPushMatrix();

	//glScalef(0.1, 0.1, 0.1);
	glGetIntegerv(GL_VIEWPORT, viewport);            /* 获取三个矩阵 */
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	glPopMatrix();

	winx = x;
	winy = scr_h - y;

	glReadPixels((int)winx, (int)winy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);            /* 获取深度 */
	gluUnProject(winx, winy, winz, mvmatrix, projmatrix, viewport, &posx, &posy, &posz);    /* 获取三维坐标 */
	int xx = changePos(posx);
	int yy = changePos(posy);
	int zz = changePos(posz);
	if (c == GLUT_LEFT_BUTTON) {
		cube_mgr.insertCube(TexCube(xx, yy, zz, 1.0f, Tnt));
	}
	else {
		torch_arr.push_back(Point(xx, yy, zz));
	}
}

/**
* @brief 经典的飞机模型
*/
void plane() {
	if (flying == true && spining == true) 
	{
		zRot = (zRot - 6) % 360;
		z_air += fly_speed;
		if (z_air > fly_distance) 
		{
			flying = false;
			spining = false;
		}
	}

	glPushMatrix();

	glTranslatef(x_air, y_air, z_air);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);  // 绕X轴旋转
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);  // 绕Y轴旋转
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);  // 绕Z轴旋转

										// 飞机
	glBegin(GL_TRIANGLES);
	// 机头
	glColor3f(0.79f, 0.08f, 0.14f);  // 深红
	glVertex3f(-15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, 60.0f / 100);

	glColor3f(0.97f, 0.29f, 0.30f);  // 粉红
	glVertex3f(-15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, 60.0f / 100);
	glVertex3f(0.0f / 100, 15.0f / 100, 30.0f / 100);

	glColor3f(1.0f, 0.51f, 0.0f);  // 橘黄
	glVertex3f(0.0f / 100, 0.0f / 100, 60.0f / 100);
	glVertex3f(15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 15.0f / 100, 30.0f / 100);

	// 机身
	glColor3f(1.0f, 1.0f, 0.0f);  // 明黄
	glVertex3f(-15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 15.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);

	glColor3f(1.0f, 1.0f, 0.0f);  // 明黄
	glVertex3f(0.0f / 100, 15.0f / 100, 30.0f / 100);
	glVertex3f(15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);

	glColor3f(0.73f, 0.0f, 0.57f);  // 紫色
	glVertex3f(15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(-15.0f / 100, 0.0f / 100, 30.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);

	// 机尾巴
	glColor3f(0.79f, 0.08f, 0.14f);  // 深红
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 15.0f / 100, -70.0f / 100);

	glColor3f(0.97f, 0.29f, 0.30f);  // 粉红
	glVertex3f(-15.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);

	glColor3f(1.0f, 0.51f, 0.0f);  // 橘黄
	glVertex3f(15.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);

	// 由于背面被消除，背面再画一次
	glColor3f(0.73f / 100, 0.0f / 100, 0.57f);  // 紫色
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);
	glVertex3f(0.0f / 100, 15.0f / 100, -70.0f / 100);


	glColor3f(1.0f, 1.0f, 0.0f);  // 明黄
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);
	glVertex3f(-15.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);

	glColor3f(1.0f, 1.0f, 0.0f);  // 明黄
	glVertex3f(0.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(15.0f / 100, 0.0f / 100, -70.0f / 100);
	glVertex3f(0.0f / 100, 0.0f / 100, -53.0f / 100);

	// 机翼下侧面
	glColor3ub(158, 196, 246 / 100);  // 紫蓝
	glVertex3f(0.0f / 100, 2.0f / 100, 27.0f / 100);
	glVertex3f(-60.0f / 100, 2.0f / 100, -8.0f / 100);
	glVertex3f(60.0f / 100, 2.0f / 100, -8.0f / 100);

	// 左翼
	glColor3f(0.2f, 0.08f, 0.69f);  // 深蓝
	glVertex3f(60.0f / 100, 2.0f / 100, -8.0f / 100);
	glVertex3f(0.0f / 100, 7.0f / 100, -8.0f / 100);
	glVertex3f(0.0f / 100, 2.0f / 100, 27.0f / 100);

	// 右翼
	glColor3f(0.2f, 0.08f, 0.69f);  // 深蓝
	glVertex3f(0.0f / 100, 2.0f / 100, 27.0f / 100);
	glVertex3f(0.0f / 100, 7.0f / 100, -8.0f / 100);
	glVertex3f(-60.0f / 100, 2.0f / 100, -8.0f / 100);

	// 机翼后侧面
	glColor3ub(192, 192, 192);
	glVertex3f(60.0f / 100, 2.0f / 100, -8.0f / 100);
	glVertex3f(-60.0f / 100, 2.0f / 100, -8.0f / 100);
	glVertex3f(0.0f / 100, 7.0f / 100, -8.0f / 100);

	glEnd();


	glPopMatrix();

}

/**
* @人物运动相关函数
*/
void human()
{
	//跳跃函数
	if (jumping == true && man.y < max_height && falling == false)
	{
		man.y += jump_speed;
		jump_speed -= 0.05;
		if (jump_speed <= 0) 
		{
			falling = true;
			jumping = false;
			jump_speed = 0.3;
		}
	}
	if (falling == true) 
	{
		man.y -= fall_speed;
		fall_speed += 0.05;
		if (man.y - 0.05 <= ground_height) 
		{
			fall_speed = 0.3;
			falling = false;
		}
	}


	// 人
	glPushMatrix();
	glTranslatef(man.x, man.y, man.z);
	glRotatef(man.vangle, 0, 1, 0);

	////////////////////////////Hair//////////////////////////
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(man.hair.x, man.hair.y, man.hair.z);
	glScalef(man.hair_scale.x, man.hair_scale.y, man.hair_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	///////////////////////////Head////////////////////////////
	// head
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.head.x, man.head.y, man.head.z);
	glScalef(man.head_scale.x, man.head_scale.y, man.head_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// left eye
	glColor3f(0.9f, 0.9f, 0.9f);
	glPushMatrix();
	glTranslatef(man.eyeL.x, man.eyeL.y, man.eyeL.z);
	glScalef(man.eye_scale.x, man.eye_scale.y, man.eye_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// right eye
	glColor3f(0.9f, 0.9f, 0.9f);
	glPushMatrix();
	glTranslatef(man.eyeR.x, man.eyeR.y, man.eyeR.z);
	glScalef(man.eye_scale.x, man.eye_scale.y, man.eye_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	// mouth
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(man.mouth.x, man.mouth.y, man.mouth.z);
	glScalef(man.mouth_scale.x, man.mouth_scale.y, man.mouth_scale.z);
	glutSolidCube(1);
	glPopMatrix();


	///////////////////////////Body////////////////////////////
	glColor3f(0.0f, 206.0 / 255, 209.0 / 255);
	glPushMatrix();
	glTranslatef(man.body.x, man.body.y, man.body.z);
	glScalef(man.body_scale.x, man.body_scale.y, man.body_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	/////////////////////////Leg/////////////////////////
	//Left leg
	glColor3f(67.0 / 255, 110.0 / 255, 238.0 / 255);
	glPushMatrix();
	glTranslatef(man.thighL.x, 2 * man.thighL.y, man.thighL.z);
	glRotatef(man.ltangle, 0, 0, 1);
	glTranslatef(0, -man.thighR.y, 0);
	glScalef(man.thigh_scale.x, man.thigh_scale.y, man.thigh_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	//Right leg
	glColor3f(67.0 / 255, 110.0 / 255, 238.0 / 255);
	glPushMatrix();
	glTranslatef(man.thighR.x, 2 * man.thighR.y, man.thighR.z);
	glRotatef(man.rtangle, 0, 0, 1);
	glTranslatef(0, -man.thighR.y, 0);
	glScalef(man.thigh_scale.x, man.thigh_scale.y, man.thigh_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	///////////////////////Arm////////////////////////

	//Left Arm
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.armL.x, man.arm_scale.y / 2 + man.armL.y, man.armL.z);
	glRotatef(man.laangle, 0, 0, 1);
	glTranslatef(0, -man.arm_scale.y / 2, 0);
	glScalef(man.arm_scale.x, man.arm_scale.y, man.arm_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	//Right Arm
	glColor3f(139.0 / 255, 115.0 / 255, 85.0 / 255);
	glPushMatrix();
	glTranslatef(man.armR.x, man.arm_scale.y / 2 + man.armR.y, man.armR.z);
	glRotatef(man.raangle, 0, 0, 1);
	glTranslatef(0, -man.arm_scale.y / 2, 0);
	glScalef(man.arm_scale.x, man.arm_scale.y, man.arm_scale.z);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
}

/**
* @brief 设置苦力怕
*/
void setCrawler() 
{
	for (auto &itr : crawler_arr)
	{
		itr.createCrawler();
	}
}

/**
* @brief 设置火把函数
*/
void setTorch()
{
	for (auto itr : torch_arr)
	{
		lightMaterial.setTorch(itr.x, itr.y, itr.z);
	}
}

void floor() 
{
	glColor3f(1.0f, 1.0f, 1.0f);
	cube_mgr.createAllCube();
	glBindTexture(GL_TEXTURE_2D, 0);
}

//display中视角切换设置
void setting_view_person() 
{
	if (view_person == FIRST_PERSON)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cameraAt.x = man.x + 1.3 * cos(man.vangle / 180.0 * PI);
		cameraAt.y = man.y + man.head.y;
		cameraAt.z = man.z - 1.3 * sin(man.vangle / 180.0 * PI);

		lookAt.x = man.x + 5 * cos(man.vangle / 180.0 * PI);
		lookAt.y = man.y + man.hair.y - look_dy;
		lookAt.z = man.z - 5 * sin(man.vangle / 180.0 * PI);
		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == GOD) 
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (!reset_god)
		{
			cameraAt = godAt;
			lookAt.x = 10;
			lookAt.y = 0;
			lookAt.z = 0;
			reset_god = true;
		}

		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == THIRD_PERSON) 
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cameraAt.x = man.x - 10 * cos(man.vangle / 180.0 * PI);
		cameraAt.y = man.head.y + 10;
		cameraAt.z = man.z + 10 * sin(man.vangle / 180.0 * PI);

		lookAt.x = man.x;
		lookAt.y = man.hair.y;
		lookAt.z = man.z;

		gluLookAt(cameraAt.x, cameraAt.y, cameraAt.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0f, 1.0f, 0.0f);
	}
	else if (view_person == BALL)
	{
		if (trackballMove) 
		{
			glRotatef(angle, axis[0], axis[1], axis[2]);
		}
	}
}

/**
* @brief 显示函数
*/
void display() 
{
	lightMaterial.setBackGround();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	lightMaterial.setLight();

	glDisable(GL_BLEND);
	lightMaterial.setMaterial(2);
	floor();
	lightMaterial.setMaterial(0);
	human();
	lightMaterial.setMaterial(2);
	plane();
	lightMaterial.setMaterial(1);
	setCrawler();
	setTorch();
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glRasterPos2f(0.0f, 0.0f);
	//drawString("Hello, World!");
	glutSwapBuffers();

	setting_view_person();
}

/**
* @brief opengl一些初始状态的设定
*/
void init() 
{
	///设置为非平滑着色
	glShadeModel(GL_FLAT);	
	///开启深度检测
	glEnable(GL_DEPTH_TEST);	
	///开启面剔除，取消对那些看不到的面的渲染
	glEnable(GL_CULL_FACE); 
	///设置逆时针的为正面
	glFrontFace(GL_CCW);    
	///开启颜色混合
	glEnable(GL_BLEND);			
	///设置以源颜色的alpha，目标颜色的所有进行混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);		
	///开启光照，关闭则物体全是昏暗的
	glEnable(GL_COLOR_MATERIAL);		
}

/**
* @brief 刷新
*/
void refresh(int c)
{
	//center.y = -(1 - sin(-left_thigh_angle / 180.0 * PI)) * basic_size * THIGH_SCALE_X + basic_size * FOOT_SCALE_Y / 2;
	if (man.is_moving())
	{
		if (man.ltangle < -60)
		{
			left_forward = false;
		}
		else if (man.ltangle > 60)
		{
			left_forward = true;
		}
		if (man.rtangle < -60) 
		{
			right_forward = false;
		}
		else if (man.rtangle > 60) 
		{
			right_forward = true;
		}
		if (left_forward)
		{
			man.ltangle--;
			man.laangle++;
		}
		else 
		{
			man.ltangle++;
			man.laangle--;
		}
		if (right_forward) 
		{
			man.rtangle--;
			man.raangle++;
		}
		else 
		{
			man.rtangle++;
			man.raangle--;
		}

	}
	else
	{
		man.raangle = 0;
		man.laangle = 0;
		man.rtangle = 0;
		man.ltangle = 0;
	}
	///每DELAY毫秒调用一下refresh，传入参数为0
	glutTimerFunc(DELAY, refresh, 0);	
	///标记当前窗口需要重绘
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	scr_w = w;
	scr_h = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0, (GLfloat)scr_w / scr_h, near_sight, far_sight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// 鼠标空闲回调函数
void passiveMotion(int x, int y)
{
	float r = 0.06;
	man.vangle -= 0.1 * (x - last_mm_x);

	if (x < r * scr_w && last_mm_x > x)
	{
		x = (1 - r) * scr_w;
		//        SetCursorPos(x, y);
	}
	else if (x > (1 - r) * scr_w && last_mm_x < x) 
	{
		x = r * scr_w;
		//        SetCursorPos(x, y);
	}
	if (view_person == FIRST_PERSON)
	{
		look_dy += 0.01 * (y - last_mm_y);
	}
	last_mm_x = x;
	last_mm_y = y;
}

// 鼠标点击函数
void mouseButton(int button, int state, int x, int y) 
{

	if (button == GLUT_LEFT_BUTTON)
	{
		switch (state)
		{
		case GLUT_DOWN:
			if (view_person == FIRST_PERSON) 
			{
				UnProject(x, y, GLUT_LEFT_BUTTON);
			}
			cameraAt.x /= VIEW_SCALE;
			cameraAt.y /= VIEW_SCALE;
			cameraAt.z /= VIEW_SCALE;
			startMotion(x, y);
			break;
		case GLUT_UP:
			stopMotion(x, y);
			break;

		}
	}
	else if (button == GLUT_RIGHT_BUTTON) 
	{
		// 视角收缩
		if (state == GLUT_DOWN)
		{
			if (view_person == FIRST_PERSON) 
			{
				UnProject(x, y, GLUT_RIGHT_BUTTON);
			}
			cameraAt.x *= VIEW_SCALE;
			cameraAt.y *= VIEW_SCALE;
			cameraAt.z *= VIEW_SCALE;
		}
	}
	TexCube *newCube = new TexCube(man.x + 1, man.y, man.z + 1, 1.0f, Stone);
	newCube->createCube();
}

// 鼠标移动回调函数
void mouseMotion(int x, int y) 
{

	trackball_ptov(x, y, scr_w, scr_h, curPos);
	trackMouse();
	glutPostRedisplay();
}

// 空闲回调函数
void idle() 
{
	// 如果鼠标发生移动，则后面自动转动
	if (redrawContinue) glutPostRedisplay();
}

// 键盘输入
void control(unsigned char key, int x, int y) 
{
	switch (key) 
	{
	case 'p':
		cout << man.vangle << endl;
		break;
	case 32:
		jumping = true;
		break;
	case 'a':
		man.move = true;
		man.x -= man.speed * sin(man.vangle / 180 * PI);
		man.z -= man.speed * cos(man.vangle / 180 * PI);
		break;
	case 'd':
		man.move = true;
		man.x += man.speed * sin(man.vangle / 180 * PI);
		man.z += man.speed * cos(man.vangle / 180 * PI);
		break;
	case 'w':
		man.move = true;
		man.x += man.speed * cos(man.vangle / 180 * PI);
		man.z -= man.speed * sin(man.vangle / 180 * PI);
		break;
	case 's':
		man.move = true;
		man.x -= man.speed * cos(man.vangle / 180 * PI);
		man.z += man.speed * sin(man.vangle / 180 * PI);
		break;
	case 'h':
		reset_god = false;
		if (view_person >= 3) 
		{
			view_person = 0;
		}
		else 
		{
			view_person++;
		}
		break;
	case 'r':
		if (view_person != 3) 
		{
			last_view = view_person;
			view_person = 3;
		}
		else 
		{
			view_person = last_view;
		}
		break;
	case 'z':
		x_air = 8.0;
		y_air = 8.0;
		z_air = -10.0;
		zRot = 0;
		flying = false;
		spining = false;
		break;
	case 'f':
		flying = true;
		spining = true;
		break;
	case 27:
		exit(0);
	default:
		break;
	}

}

// 按键松开
void controlup(unsigned char key, int x, int y) 
{
	switch (key) 
	{
	case 'w':
		man.move = false;
		break;
	case 's':
		man.move = false;
		break;
	case 'a':
		man.move = false;
		break;
	case 'd':
		man.move = false;
		break;
	}
}

/**
* @brief 方块(地图)初始化，加载纹理贴图
*/
void initCube()
{
	///启用2D纹理和材质初始化
	glEnable(GL_TEXTURE_2D); 
	///开启深度检测
	glEnable(GL_DEPTH_TEST);	
	///纹理参数设置，设置GL_NEAREST速度较快，设置GL_REPEAT使纹理重复
	glTexParameterf(GL_NEAREST, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	refresh(0);

	Cube::initCubeTexture();

	Chunk chunk_test;
	chunk_test.creat_chunk(cube_mgr);

	/*
	int num_cube = 30;

	for (int i = num_cube; i >= (-num_cube); i--)
	{
		for (int j = num_cube; j >= (-num_cube); j--) 
		{
			cube_mgr.insertCube(TexCube(i, 0, j, 1.0f, Soil));
		}
	}
	*/
	//////////////////////地面//////////////////////////
	
	cube_mgr.insertCube(TexCube(0, 1, 0, 1.0f, Table));

	cube_mgr.buildPool(8, 1, -10);
	cube_mgr.buildPyramid(10, 10, 10);
	cube_mgr.buildDiamondBuilding(-8, 1, 1);
	//种树----
	for (int i = 1; i <= 6; i++) 
	{
		cube_mgr.buildTree(-1, 1, -4 * i);
		cube_mgr.buildTree(19, 1, -4 * i);
	}
	//cube_mgr.createAllCube();
}

// 播放音乐
void music() {
	//    MCI_OPEN_PARMS op;                                     //播放背景音乐
	//    op.dwCallback = NULL;
	//    op.lpstrAlias = NULL;
	//    op.lpstrDeviceType = "MPEGAudio";
	//    op.lpstrElementName = "music.mp3";
	//    op.wDeviceID = NULL;
	//    UINT rs;
	//    rs = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD) & op);
	//    if (rs == 0) {
	//        MCI_PLAY_PARMS pp;
	//        pp.dwCallback = NULL;
	//        pp.dwFrom = 0;
	//        //mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&pp);
	//        mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD) & pp);
	//
	//    }
}

/**
* @brief 初始化除方块外的其他物体
*/
void initOther()
{
	for (int i = 0; i < 10; i++)
	{
		crawler_arr.push_back(Crawler(8, 1, -10 - i));
	}

}

/**
* @brief 设定人物的初始位置（出生点^_^）
* @return hhhhhhhhhh
*/
void setPosition()
{

	man.x = 0;	
	man.y = 0;
	man.z = 0;

	x_air = 8.0f;
	y_air = 8.0f;
	z_air = -10.0f;
}


/**
* @brief main函数
* @param argc:参数数量  *argv[]参数数组
* @return 呵呵呵呵呵呵
*/
int main(int argc, char *argv[]) 
{
	//start 初始窗口设置
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	GLint screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	GLint screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition((screenWidth - WindowWidth) / 2, (screenHeight - WindowHeight) / 2);
	glutCreateWindow("我的世界");
	//end

	int length=256;
	int ****a = new int***[length];
	for (size_t num_chunk = 0; num_chunk < 50; num_chunk++)
	{
		a[num_chunk] = new int**[16];
		for (size_t i = 0; i < 16; i++)
		{
			a[num_chunk][i] = new int*[16];
			for (size_t j = 0; j < 16; j++)
			{
				a[num_chunk][i][j] = new int[256];
			}
		}
	}
	

	init();
	//lastTime = clock();    //启动时首次设定当前时间
	setPosition();
	//ShowCursor(false);
	initCube();
	initOther();

	glutDisplayFunc(display);
	///窗口大小改变时不改变游戏物体比例
	glutReshapeFunc(reshape);
	///当鼠标移动时，并有没鼠标键按下时的回调函数
	glutPassiveMotionFunc(passiveMotion);
	///鼠标点击的回调函数
	glutMouseFunc(mouseButton);
	///当鼠标移动时，鼠标键按下时的回调函数
	glutMotionFunc(mouseMotion);
	///键盘按键弹起时的回调函数
	glutKeyboardUpFunc(controlup);
	///键盘回调函数
	glutKeyboardFunc(control);

	glutMainLoop();

	return 0;
}

