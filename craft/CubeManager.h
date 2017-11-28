#pragma once

#include"TexCube.h"
#include<map>
#include<iostream>
#include<string>



/**
* @brief 方块管理类
*/
class CubeManager 
{
public:
	void insertCube(const TexCube &texcube);

	CubeManager() {};

	~CubeManager() {};

	void createAllCube();

	/**
	* @brief 构建沙漠神庙
	*/
	void buildPyramid(int x, int y, int z);

	void buildannexBuilding(int x, int y, int z);

	/**
	* @brief 构建水池
	*/
	void buildPool(int x, int y, int z);
	/**
	* @brief 构建树木
	*/
	void buildTree(int x, int y, int z);

	
	void buildDiamondBuilding(int x, int y, int z);

	bool isExist(std::string);

	std::map<std::string, TexCube> cubeMap;
};

