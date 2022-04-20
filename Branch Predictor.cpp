//**************************************************************************//
// Project  ECE 483 Branch Predictor                                        //
// Author:  Ryan Bride                                      //
// File:    Branch Predictor.cpp                                            //
// Note:                                                                    //
//  This program is very hacky, while it works, the entire project,         // 
//  could use refactoring. This is expecially of the file_obj class         //
//  which effectively only exist to comparmentalize file.open & file.close  // 
//  functions. Another needed refactor would be the g_share function,       //
//  which was hastly hacked together. In contrast the Bi_Modal function,    //
//  while imperfect, has seen some refactoring & runs very efficiently,     //
//  processing 2,000,000 entries in under half a second :)                  //
//         - Ryan Bride                                                     //
//**************************************************************************//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "file_obj.h"
#include "bi_modal.h"
#include "g_share.h"
using namespace std;

int main() {
    while(true) {
        int mode;
        cout << "Enter 1 for Bi_Modal, 2 for Gshare, 3 for Exit \n>: ";
        cin >> mode; cout << '\n';   
        switch (mode) {
            case 1:
                bi_modal();
                break;
            case 2:
                g_share();
                break;
            case 3:
                return 0;
            default:
                cout << "\nInvalid Entry";
                break;
        }
    }
    return 0;
}