#include "FileCompress.h"
#include <iostream>
using namespace std;

FileCompress::FileCompress() {
	for (int i = 0; i < 256; ++i) 
		fileByteInfo[i].ch = i;
}

void FileCompress::WriteHead(FILE* fout, const string& filepath) {
	//1.��ȡԴ�ļ���׺
	string postFix = filepath.substr(filepath.rfind('.'));
	postFix += '\n';
	fwrite(postFix.c_str(), 1, postFix.size(), fout);
	//2.�����ֽ�Ƶ����Ϣ����Чͷ��Ϣ������
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
	//1.ͳ��Դ�ļ���ÿ���ֽڳ��ֵĴ���
	FILE* pf = fopen(filepath.c_str(), "rb");
	if (nullptr == pf) {
		cout << "���ļ�ʧ��!" << endl;
		return false;
	}

	//�ļ���С��֪������Ҫѭ����ȡԴ�ļ�����
	uchar readBuff[1024];
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) 
			break;
		for (size_t i = 0; i < rdsize; ++i) //ֱ�Ӷ�ֵ��
			++fileByteInfo[readBuff[i]].appearCount;
	}
	///2.����ͳ�ƽ������huffman��
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;//��Ҫ�����ִ���Ϊ0���ֽ��޳�
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);
	///3.����huffman����ȡÿ���ֽڵı���
	GenerateHuffmanCode(ht.GetRoot());
	///4.д���ѹ������Ϣ
	string filename = filepath;
	filename.erase(filename.begin()+filename.find('.'), filename.end());
	filename += ".foreb";
	FILE* fout = fopen(filename.c_str(), "wb");
	WriteHead(fout, filepath);
	///5.��дԴ�ļ�
	fseek(pf, 0, SEEK_SET);//or rewind(pf);
	uchar ch = 0;
	uchar bitcnt = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize)
			break;
		//�����д�ֽ�--����ѹ������ļ���
		for (size_t i = 0; i < rdsize; ++i) {
			string& strCode = fileByteInfo[readBuff[i]].strCode;
			//���ַ�����ʽ�Ķ��������ֽ��з���
			for (size_t j = 0; j < strCode.size(); ++j) {
				ch <<= 1;//��λ��������λ��0
				if ('1' == strCode[j])
					ch |= 1;
				++bitcnt;
				//��ch�е�8������λ�����֮����Ҫд���ļ�
				if (8 == bitcnt) {
					fputc(ch, fout);
					bitcnt = 0;
				}
			}
		}
	}
	///��⣺ch����8bit��û��д���ļ��������Ҫ���
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
		while (parent != nullptr) {//��1��0
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
	/// 1.��ѹ���ļ��ж�ȡ��ѹ��������Ϣ
	FILE* fin = fopen(filepath.c_str(), "rb");
	if (nullptr == fin) {
		cout << "���ļ�ʧ��" << endl;
		return false;
	}
	//��ȡ��׺
	string PostFix;
	GetLine(fin, PostFix);
	//��ȡƵ����Ϣ����
	string StrContent;
	GetLine(fin, StrContent);
	size_t linecnt = atoi(StrContent.c_str());
	//ѭ����ȡlinecnt�У���ȡƵ����Ϣ
	for (size_t i = 0; i < linecnt; ++i) {
		GetLine(fin, StrContent);
		if ("" == StrContent) {//�������з�
			StrContent += '\n';
			GetLine(fin, StrContent);
		}
		fileByteInfo[(uchar)StrContent[0]].appearCount = atoi(StrContent.c_str()+2);
		StrContent = "";
	}
	/// 2.�ָ�Huffman��
	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);

	/// 3.��ȡѹ�����ݣ����huffman�����н�ѹ��
	string filename = filepath;
	filename.erase(filename.begin() + filename.find('.'), filename.end());
	filename += "_copy";
	filename += PostFix;
	FILE* fout = fopen(filename.c_str(), "wb");
	if (fout == nullptr) {
		cout << "��ѹʧ��!" << endl;
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