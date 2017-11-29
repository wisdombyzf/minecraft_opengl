#include<iostream>
#include<map>
#include<vector>
#include<string>

using namespace std;

int main()
{
	map<int,string> t;
	for (size_t i = 0; i < 16; i++)
	{
		t[i] = "²âÊÔ" + to_string(i);
	}


	for (auto it = t.begin(); it != t.end();it++)
	{
		cout << it->second << endl;
	}
	/*
	for (auto it:t)
	{
		cout << it.second << endl;
	}
	*/
	system("pause");
}