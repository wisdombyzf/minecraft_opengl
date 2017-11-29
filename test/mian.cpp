#include<iostream>
#include<map>
#include<vector>
#include<string>

using namespace std;

class hh
{
public:
	hh()
	{
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

private:
	///一个chunk的长
	const size_t chunk_x = 16;
	///一个chunk的高
	const size_t chunk_y = 256;
	///一个chunk的深(按屏幕坐标系)
	const size_t chunk_z = 16;
	///数据储存数组
	std::vector<std::vector<std::vector<size_t>>> chunk_data;
};


int main()
{
	std::vector<std::vector<hh>> word_data;
	word_data.resize(10);
	for (size_t i = 0; i < 10; i++)
	{
		word_data[i].resize(10);
	}
	system("pause");
}