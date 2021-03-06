# FileCompresser
基于Huffman的压缩程序

基于Huffman算法完成字节层面的压缩。

问题/原因/解决方法：
问题1：多行数据恢复失败
原因：在恢复时未考虑多行数据，导致从压缩文件读取到"\n"的编码时，恢复出错。
解决方法：恢复时处理"\n"即可

问题2：文件内容含汉字，压缩后恢复不成功（汉字变乱码）
原因：ASCII码表中并没有汉字，且汉字并不由一个ASCII表表示。在编程过程中，字符一直使用的
char（0-255）,汉字实际的编码超出了这个范围，导致恢复乱码
解决方法：使用unsigned char类型替换char类型，扩大码表范围。

问题3：复杂文件可以完成压缩，但只能部分解压，丢失数据 
原因：打开文件方式不正确，fopen函数以"w"方式打开文件，文件经Huffman字节压缩算法压缩后，
字节层面会出现"0xff"，而"0xff"恰好是fopen函数作为文件结尾标志的字符，使得实际文件并没有读取
到完整。
解决方法：将文件读写方式改为二进制方式："w"->"wb" , "r"->"rb"