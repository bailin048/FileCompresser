#include "HuffmanTree.h"
#include "FileCompress.h"
#include <iostream>
using namespace std;

int main() {
	bool flag = true;
	FileCompress fc;
	while (flag) {
		int choice = 3;
		cout << "请选择需要执行的操作:\n1.压缩  2.解压 3.退出\n请输入选择#";
		cin >> choice;
		switch (choice){
		case 1: {
			string filepath;
			cout << "请输入目标文件所在路径：";
			cin >> filepath;
			fc.CompressFile(filepath);
			cout << "压缩成功！\n" << endl;
		}			break;
		case 2: {
			string filepath;
			cout << "请输入目标文件所在路径：";
			cin >> filepath;
			fc.UnCompressFile(filepath);
			cout << "解压成功！\n" << endl;
		}break;
		case 3: flag = false; break;
		default:
			cout << "输入错误!请重新选择#"; 
			break;
		}
	}
	return 0;
}