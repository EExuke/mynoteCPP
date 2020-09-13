/**************************************************************************** **
 * Copyright (CPP) 2019 EExuke Communications, Inc.
 **************************************************************************** **/

/**************************************************************************** **
 * 使用内联函数来返回两个数中的最大值：
 **************************************************************************** **/

#include <iostream>
 
using namespace std;

// 内联函数
inline int Max(int x, int y)
{
	return (x > y)? x : y;
}

// 程序的主函数
int main( )
{
	cout << "Max (20,10): " << Max(20,10) << endl;
	cout << "Max (0,200): " << Max(0,200) << endl;
	cout << "Max (100,1010): " << Max(100,1010) << endl;
	return 0;
}