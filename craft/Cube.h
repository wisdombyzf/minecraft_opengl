#pragma once
#include <iostream>
#include <cmath>
#include<gl\glut.h>
#include "Point.h"

#define BMP_Header_Length 54

class Cube 
{
public:
	Cube(float, float, float, float, GLuint, GLuint, GLuint);

	void createCube();

	static int power_of_two(int n);

	static GLuint load_texture(const char *file_name);

	static void initCubeTexture();

	~Cube();

public:
	float _x, _y, _z, _size;
public:
	static GLuint texRedStone;
	static GLuint texGrass;
	static GLuint texSoil;
	static GLuint texStone;
	static GLuint texWater;
	static GLuint texWood;
	static GLuint texDirt;
	static GLuint texBrick;
	static GLuint texTabletop;
	static GLuint texTableside;
	static GLuint texDiamond;
	static GLuint texTnttop;
	static GLuint texTntside;
	static GLuint texTreetop;
	static GLuint texTreeside;
	static GLuint texLeaf;
	static GLuint texBookshelf;
	static GLuint texRedSand;
	static GLuint texSand;
protected:
	Cube() {};
	GLuint _top, _bot, _side;
};


