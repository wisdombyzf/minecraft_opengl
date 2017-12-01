/**
* @file main.c
* @author zf
* @date 2017/11/26
* @detail 尝试利用opengl去复刻minecraft
* @TODO 地形生成，能加载minecraft的材质包，解决鼠标漂移问题，破坏方块，方块状态的记录
*/
#define  _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<map>
#include<string>
#include"Crawler.h"
//#include "visualBall.h"
#include "CubeManager.h"
#include "LightMaterial.h"
#include"Man.h"
#include"Chunk.h"
#include"coll_dete.h"
#include"Camera.h"
#include"Skybox.h"

using namespace std;
////参数
#define DELAY 12
#define NUM_OF_LINE 32
#define BLOCK_SIZE 1
#define PI 3.1415926535898
#define MAX_CHAR 128


Chunk chunk_test;
Camera camera;
CSkyBox skybox;
///
float near_sight = 0.1f;
float far_sight = 100;
int scr_w;
int scr_h;
extern float curPos[3];

vector<Point> torch_arr;




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



/**
* @brief 显示函数
*/
void display() 
{

	lightMaterial.setBackGround();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	lightMaterial.setLight();
	//TODO  可能纹理贴反了
	skybox.CreateSkyBox(man.x, man.y, man.z, 1.0, 0.5, 1.0);

	glDisable(GL_BLEND);
	lightMaterial.setMaterial(2);
	
	floor();
	lightMaterial.setMaterial(0);
	man.refresh();
	lightMaterial.setMaterial(1);
	setCrawler();
	setTorch();
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glRasterPos2f(0.0f, 0.0f);
	//drawString("Hello, World!");
	glutSwapBuffers();
	camera.setting_view_person(man);
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
* @TODO   待解决
*/
void refresh(int c)
{
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

// 鼠标空闲回调函数.......
void passiveMotion(int x, int y)
{
	camera.unname(x, y, scr_w, man);
}


// 鼠标点击函数
void mouseButton(int button, int state, int x, int y) 
{
	camera.mouse_Button(button, state, x, y, scr_h);
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
/*
void idle() 
{
	// 如果鼠标发生移动，则后面自动转动
	if (redrawContinue) glutPostRedisplay();
}*/

// 键盘输入
void control(unsigned char key, int x, int y) 
{
	Collision check;
	cout << "X:" << man.x << "  Z:" << man.z << endl;
	switch (key) 
	{
	case 'p':
		cout << man.vangle << endl;
		break;
	case 32:
		man.jump();
		break;
	case 'a':
		man.left(chunk_test);
		break;
	case 'd':
		man.right(chunk_test);
		break;
	case 'w':
		man.forward(chunk_test);
		break;
	case 's':
		man.back(chunk_test);
		break;
	case 'h':
		camera.change_view();
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
	Cube::initCubeTexture();
	chunk_test.creat_chunk(cube_mgr);
	//chunk_test.print_test();

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
	cube_mgr.buildPool(8, 1, 10);
	cube_mgr.buildPyramid(30, 1, 10);
	cube_mgr.buildDiamondBuilding(8, 1, 1);
	//种树----
	for (int i = 1; i <= 6; i++) 
	{
		cube_mgr.buildTree(1, 1, 4 * i);
		cube_mgr.buildTree(19, 1, 4 * i);
	}
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
	refresh(0);
	for (int i = 0; i < 10; i++)
	{
		crawler_arr.push_back(Crawler(8, 1, -10 - i));
	}

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
	cout << "话说如果没发生改变的。。。" << endl;
	init();
	//lastTime = clock();    //启动时首次设定当前时间
	man.setLocation(6, 1, 6);
	//ShowCursor(false);
	initCube();
	initOther();
	glutDisplayFunc(display);
	//窗口大小改变时不改变游戏物体比例
	glutReshapeFunc(reshape);
	//当鼠标移动时，并有没鼠标键按下时的回调函数
	glutPassiveMotionFunc(passiveMotion);
	//鼠标点击的回调函数
	glutMouseFunc(mouseButton);
	//当鼠标移动时，鼠标键按下时的回调函数
	glutMotionFunc(mouseMotion);
	//键盘按键弹起时的回调函数
	glutKeyboardUpFunc(controlup);
	//键盘回调函数
	glutKeyboardFunc(control);

	glutMainLoop();

	system("pause");
	return 0;
}

