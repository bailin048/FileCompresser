#include "FileCompress.h"
#include "HuffmanTree.h"
#include <iostream>
using namespace std;

FileCompress::FileCompress() {
	for (int i = 0; i < 256; ++i) 
		fileByteInfo[i].ch = i;
}

bool FileCompress::CompressFile(const string& filepath) {
	//1.ͳ��Դ�ļ���ÿ���ֽڳ��ֵĴ���
	FILE* pf = fopen(filepath.c_str(), "r");
	if (nullptr == pf) {
		cout << "���ļ�ʧ��!" << endl;
		return false;
	}

	//�ļ���С��֪������Ҫѭ����ȡԴ�ļ�����
	char readBuff[1024];
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) 
			break;
		for (size_t i = 0; i < rdsize; ++i) //ֱ�Ӷ�ֵ��
			++fileByteInfo[readBuff[i]].appearCount;
	}
	fclose(pf);
	///2.����ͳ�ƽ������huffman��
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;//��Ҫ�����ִ���Ϊ0���ֽ��޳�
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);

	return true;
}

bool FileCompress::UnCompressFile(const string& filepath) {
	return true;
}