#include "FileCompress.h"
#include "HuffmanTree.h"
#include <iostream>
using namespace std;

FileCompress::FileCompress() {
	for (int i = 0; i < 256; ++i) 
		fileByteInfo[i].ch = i;
}

bool FileCompress::CompressFile(const string& filepath) {
	//1.统计源文件中每个字节出现的次数
	FILE* pf = fopen(filepath.c_str(), "r");
	if (nullptr == pf) {
		cout << "打开文件失败!" << endl;
		return false;
	}

	//文件大小不知道，需要循环获取源文件内容
	char readBuff[1024];
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) 
			break;
		for (size_t i = 0; i < rdsize; ++i) //直接定值法
			++fileByteInfo[readBuff[i]].appearCount;
	}
	fclose(pf);
	///2.根据统计结果创建huffman树
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;//需要将出现次数为0的字节剔除
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);

	return true;
}

bool FileCompress::UnCompressFile(const string& filepath) {
	return true;
}