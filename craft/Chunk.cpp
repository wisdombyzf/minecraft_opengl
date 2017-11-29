#include "Chunk.h"
#pragma once


Chunk::Chunk()
{
	//初始化vector多维数组
	chunk_data.resize(chunk_x);
	for (size_t i = 0; i < chunk_x; i++)
	{
		chunk_data[i].resize(chunk_y);
		for (size_t j = 0; j < chunk_y; j++)
		{
			chunk_data[i][j].resize(chunk_z);
		}
	}
}

Chunk::~Chunk()
{
}

void Chunk::print_test()
{
	for (size_t i = 0; i < chunk_x; i++)
	{
		for (size_t j = 0; j < chunk_y; j++)
		{
			for (size_t k = 0; k < chunk_z; k++)
			{
				std::cout << chunk_data[i][j][k];
			}
		}
	}
}

bool Chunk::creat_chunk(CubeManager &cube_mgr)
{
	;
	for (size_t i = 0; i < chunk_x; i++)
	{
		for (size_t j = 0; j < chunk_y; j++)
		{
			for (size_t k = 0; k < chunk_z; k++)
			{
				if (j<10)
				{
					chunk_data[i][j][k] = 1;
					cube_mgr.insertCube(TexCube(i,j, k,1.0f,Soil));
				}
			}
		}
	}
	return false;
}

bool Chunk::set_data(size_t x, size_t y, size_t z, size_t new_block)
{
	if (x>chunk_x || y>chunk_y || z>chunk_z)
	{
		return false;
	}
	else
	{
		chunk_data[x][y][z] = new_block;
		return true;
	}
}

int Chunk::get_data(size_t x, size_t y, size_t z)
{
	if (x>chunk_x || y>chunk_y || z>chunk_z)
	{
		return -1;
	}
	else
	{
		return (int)chunk_data[x][y][z];
	}
}
