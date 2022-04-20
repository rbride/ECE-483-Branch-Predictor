#include "file_obj.h"
using namespace  std;

void FileObj::OpenFile() {
    if(File.good()) {
        File.open(FileName);
    }
    return;
}

void FileObj::CloseFile() {
    File.close();
    return; 
}

bool FileObj::EndOfFile() {
    return File.eof();
}

int FileObj::Parse_Index() {
    string line_read, index_str;
    std::getline(File, line_read);
    char last_char = line_read[line_read.size() - 1];
    taken_not_taken = (last_char == 't') ? 116 : 110; 
    size_t pos_of_space = line_read.find(' ');
    index_str = line_read.substr(0, pos_of_space);
    int index = stoi(index_str, nullptr, 16);
    index_str = "";
    return index;
}