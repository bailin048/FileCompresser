#pragma once
#include <string>
#include "HuffmanTree.h"
#include <algorithm>
#pragma warning(disable:4996)
using namespace std;

typedef unsigned char uchar;
struct ByteInfo {
	uchar ch;
	int appearCount = 0;//chƵ��
	string strCode;//ch��Ӧ����

	ByteInfo operator+(const ByteInfo& b)const {
		ByteInfo tmp;
		tmp.appearCount = b.appearCount + appearCount;
		return tmp;
	}

	bool operator>(const ByteInfo& b)const {
		return appearCount > b.appearCount;
	}

	bool operator!=(const ByteInfo& b)const {
		return appearCount != b.appearCount;
	}

	bool operator==(const ByteInfo& b)const {
		return appearCount == b.appearCount;
	}
};
class FileCompress {
public:
	FileCompress();
	bool CompressFile(const std::string& filepath);
	bool UnCompressFile(const std::string& filepath);
private:
	void WriteHead(FILE* fout, const string& filepath);
	void GetLine(FILE* fin, string& StrContent);
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root);
	//�ļ������ڴ����������ֽڷ�ʽ��256�֣��洢��
	//ֻ��һ������256��ByteInfo���͵������������ֽڵ������Ϣ
	ByteInfo fileByteInfo[256];
};