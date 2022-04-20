# ECE-483-Branch-Predictor
Simple gshare and bimodal branch predictor terminal emulator and testing tool

This program is very hacky, while it works, the entire project, could use refactoring. This is expecially of the file_obj class which effectively only exist to comparmentalize file.open & file.close functions. Another needed refactor would be the g_share function, which was hastly hacked together. In contrast the Bi_Modal function, while imperfect, has seen some refactoring & runs very efficiently, processing 2,000,000 entries in under half a second :)                  
