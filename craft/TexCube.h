#pragma once
#include"Cube.h"
enum CubeTex {
	Wood,
	Water,
	Soil,
	Stone,
	RedStone,
	Dirt,
	Brick,
	Table,
	Diamond,
	Tnt,
	Tree,
	Leaf,
	Bookshelf,
	RedSand,
	Sand
};

//纹理相关设置
class TexCube : public Cube 
{
public:
	TexCube(float, float, float, float, CubeTex);
	TexCube() {};
	void createCube();
	~TexCube();
private:
	GLuint _top;
	GLuint _bot;
	GLuint _side;
};


