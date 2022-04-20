#ifndef FILE_OBJ_H
#define FILE_OBJ_H
#include <iostream>
#include <fstream>
#include <string> 

class FileObj : public std::ifstream {
private:
	//Private Variables
	std::string FileName;
	std::string line_read;
	std::ifstream File;
public:
	//Construction Zone
	FileObj(const std::string& filename) : FileName(filename), taken_not_taken(0) {}
	~FileObj() { File.close(); }
	//Public Functions
	int Parse_Index();
    void OpenFile();
	void CloseFile();
	bool EndOfFile();
	//Public Variables
	int taken_not_taken;
};
#endif