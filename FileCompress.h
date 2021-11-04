#pragma once
#include <string>
#pragma warning(disable:4996)
using namespace std;

struct ByteInfo {
	char ch;
	int appearCount;//ch频次
	string strCode;//ch对应编码

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

	//文件最终在磁盘上是以字节方式（256种）存储的
	//只需一个包含256个ByteInfo类型的数组来保存字节的相关信息
	ByteInfo fileByteInfo[256];
};