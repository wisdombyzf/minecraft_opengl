#pragma once
#include<vector>
#include"Point.h"
/**
* @brief 碰撞检测相关类
*/

class Collision
{
public:
	Collision();
	~Collision();

	/**
	* @param a,b为物体的两对角顶点
	* @brief 碰撞检测,类AABB算法
	*/
	bool coll_dete(Point a, Point b);

	/**
	* @brief 坐标转化，将opengl中的坐标对应到某一chunk中的坐标
	*/

private:


};

