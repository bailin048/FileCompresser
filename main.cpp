#include "HuffmanTree.h"
#include "FileCompress.h"
#include <iostream>
using namespace std;

int main() {
	bool flag = true;
	FileCompress fc;
	while (flag) {
		int choice = 3;
		cout << "��ѡ����Ҫִ�еĲ���:\n1.ѹ��  2.��ѹ 3.�˳�\n������ѡ��#";
		cin >> choice;
		switch (choice){
		case 1: {
			string filepath;
			cout << "������Ŀ���ļ�����·����";
			cin >> filepath;
			fc.CompressFile(filepath);
			cout << "ѹ���ɹ���\n" << endl;
		}			break;
		case 2: {
			string filepath;
			cout << "������Ŀ���ļ�����·����";
			cin >> filepath;
			fc.UnCompressFile(filepath);
			cout << "��ѹ�ɹ���\n" << endl;
		}break;
		case 3: flag = false; break;
		default:
			cout << "�������!������ѡ��#"; 
			break;
		}
	}
	return 0;
}