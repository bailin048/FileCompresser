#include "FileCompress.h"
#include <iostream>
using namespace std;

FileCompress::FileCompress() {
	for (int i = 0; i < 256; ++i) 
		fileByteInfo[i].ch = i;
}

void FileCompress::WriteHead(FILE* fout, const string& filepath) {
	//1.获取源文件后缀
	string postFix = filepath.substr(filepath.rfind('.'));
	postFix += '\n';
	fwrite(postFix.c_str(), 1, postFix.size(), fout);
	//2.构造字节频次信息及有效头信息总行数
	string ChAppearCnt;
	size_t linecnt = 0;
	for (size_t i = 0; i < 256; ++i) {
		if (fileByteInfo[i].appearCount > 0) {
			ChAppearCnt += fileByteInfo[i].ch;
			ChAppearCnt += ':';
			ChAppearCnt += to_string(fileByteInfo[i].appearCount);
			ChAppearCnt += '\n';
			++linecnt;
		}
	}
	string TotalLine = to_string(linecnt);
	TotalLine += '\n';
	fwrite(TotalLine.c_str(), 1, TotalLine.size(), fout);
	fwrite(ChAppearCnt.c_str(), 1, ChAppearCnt.size(), fout);
}

bool FileCompress::CompressFile(const string& filepath) {
	//1.统计源文件中每个字节出现的次数
	FILE* pf = fopen(filepath.c_str(), "rb");
	if (nullptr == pf) {
		cout << "打开文件失败!" << endl;
		return false;
	}

	//文件大小不知道，需要循环获取源文件内容
	uchar readBuff[1024];
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) 
			break;
		for (size_t i = 0; i < rdsize; ++i) //直接定值法
			++fileByteInfo[readBuff[i]].appearCount;
	}
	///2.根据统计结果创建huffman树
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;//需要将出现次数为0的字节剔除
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);
	///3.借助huffman树获取每个字节的编码
	GenerateHuffmanCode(ht.GetRoot());
	///4.写入解压所需信息
	string filename = filepath;
	filename.erase(filename.begin()+filename.find('.'), filename.end());
	filename += ".foreb";
	FILE* fout = fopen(filename.c_str(), "wb");
	WriteHead(fout, filepath);
	///5.改写源文件
	fseek(pf, 0, SEEK_SET);//or rewind(pf);
	uchar ch = 0;
	uchar bitcnt = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize)
			break;
		//编码改写字节--放置压缩结果文件中
		for (size_t i = 0; i < rdsize; ++i) {
			string& strCode = fileByteInfo[readBuff[i]].strCode;
			//将字符串格式的二进制往字节中放置
			for (size_t j = 0; j < strCode.size(); ++j) {
				ch <<= 1;//高位丢弃，低位补0
				if ('1' == strCode[j])
					ch |= 1;
				++bitcnt;
				//当ch中的8个比特位填充满之后，需要写入文件
				if (8 == bitcnt) {
					fputc(ch, fout);
					bitcnt = 0;
				}
			}
		}
	}
	///检测：ch不够8bit，没有写进文件，因此需要检测
	if (bitcnt > 0 && bitcnt < 8) {
		ch <<= (8 - bitcnt);
		fputc(ch, fout);        
	}
	fclose(pf);
	fclose(fout);
	return true;
}

void FileCompress::GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root) {
	if (root == nullptr)
		return;
	if (nullptr == root->left && nullptr == root->right) {
		HuffmanTreeNode<ByteInfo>* cur = root;
		HuffmanTreeNode<ByteInfo>* parent = cur->parent;
		string& strCode = fileByteInfo[cur->weight.ch].strCode;
		while (parent != nullptr) {//左1右0
			if (cur == parent->left)
				strCode += '1';
			else
				strCode += '0';
			cur = parent;
			parent = cur->parent;
		}
		reverse(strCode.begin(), strCode.end());
	}
	GenerateHuffmanCode(root->left);
	GenerateHuffmanCode(root->right);
}
 
void FileCompress::GetLine(FILE* fin, string& StrContent) {
	uchar ch;
	while (!feof(fin)) {
		ch = fgetc(fin);
		if (ch == '\n')
			break;
		StrContent += ch;
	}
}

bool FileCompress::UnCompressFile(const string& filepath) {
	/// 1.从压缩文件中读取解压缩所需信息
	FILE* fin = fopen(filepath.c_str(), "rb");
	if (nullptr == fin) {
		cout << "打开文件失败" << endl;
		return false;
	}
	//读取后缀
	string PostFix;
	GetLine(fin, PostFix);
	//读取频次信息行数
	string StrContent;
	GetLine(fin, StrContent);
	size_t linecnt = atoi(StrContent.c_str());
	//循环读取linecnt行：获取频次信息
	for (size_t i = 0; i < linecnt; ++i) {
		GetLine(fin, StrContent);
		if ("" == StrContent) {//读到换行符
			StrContent += '\n';
			GetLine(fin, StrContent);
		}
		fileByteInfo[(uchar)StrContent[0]].appearCount = atoi(StrContent.c_str()+2);
		StrContent = "";
	}
	/// 2.恢复Huffman树
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);

	/// 3.读取压缩数据，结合huffman树进行解压缩
	string filename = filepath;
	filename.erase(filename.begin() + filename.find('.'), filename.end());
	filename += "_copy";
	filename += PostFix;
	FILE* fout = fopen(filename.c_str(), "wb");
	if (fout == nullptr) {
		cout << "解压失败!" << endl;
		return false;
	}
	uchar readBuff[1024];
	uchar bitCnt = 0;
	HuffmanTreeNode<ByteInfo>* cur = ht.GetRoot();
	const int fileSize = cur->weight.appearCount;
	int compressSize = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, fin);
		if (0 == rdsize)
			break;
		for (size_t i = 0; i < rdsize; ++i) {
			uchar ch = readBuff[i];
			bitCnt = 0;
			while (bitCnt < 8) {
				if (ch & 0x80)
					cur = cur->left;
				else
					cur = cur->right;
				++bitCnt;
				if (nullptr == cur->left && nullptr == cur->right) {
					fputc(cur->weight.ch, fout);
					cur = ht.GetRoot();
					++compressSize;
					if (compressSize == fileSize)
						break;
				}
				ch <<= 1;
			}
		}
	}
	fclose(fout);
	fclose(fin);
	return true;
}