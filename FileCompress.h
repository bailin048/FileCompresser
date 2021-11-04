#pragma once
#include <string>
#pragma warning(disable:4996)
using namespace std;

struct ByteInfo {
	char ch;
	int appearCount;//chƵ��
	string strCode;//ch��Ӧ����

	ByteInfo operator+(const ByteInfo& b)const {
		ByteInfo tmp;
		tmp.appearCount = b.appearCount + appearCount;
		return tmp;
	}

	bool operator>(const ByteInfo& b)const {
		return appearCount > b.appearCount;
	}
};
class FileCompress {
public:
	FileCompress();
	bool CompressFile(const std::string& filepath);
	bool UnCompressFile(const std::string& filepath);

	//�ļ������ڴ����������ֽڷ�ʽ��256�֣��洢��
	//ֻ��һ������256��ByteInfo���͵������������ֽڵ������Ϣ
	ByteInfo fileByteInfo[256];
};