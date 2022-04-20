#include "g_share.h"
using namespace std;

void g_share() {
    int num_predictions = 0;
    int num_mispredictions = 0;
    int hist = 0;
    //Define Files the program is working with
    string FileName, OutFileName;
    int m, n, m_i;
    cout << "File Name: ";  cin >> FileName;
    cout << "\nOutput File Name: "; cin >> OutFileName;
    cout << "\nLocal History Register Size (M): "; cin >> m;
    cout << "\nGlobal History Register Size (N): "; cin >> n;
    //Open Files
    ofstream outfile;
    outfile.open(OutFileName);
    FileObj predict_me(FileName);
    predict_me.OpenFile();
    //Local history register is basically the Bimodal register
    m_i = static_cast<int>(pow(2, m));
    vector<int> local_hist_reg(m_i, 2);

    //Main Function Loop
    while (!predict_me.EndOfFile()) {
        int index = predict_me.Parse_Index(); 
        index = (index >> 2) & (m_i-1);    //Get bi_modal index kex 
        //Determine prediction by xoring local and global
        int temp = (hist << (m-n)); 
        int new_predict = temp ^ index;
        //Determine if a misprediction  Note: 110 = 'n' 116 = 't'
        num_predictions++;
        int taken_2 = predict_me.taken_not_taken; //Determine if taken or not taken
        //Determine if taken or not taken has been correctly predicted.
        if (taken_2 == 116  && local_hist_reg[new_predict] < 2) {
            num_mispredictions = num_mispredictions+1;
        }
        else if (taken_2 == 110 && local_hist_reg[new_predict] >= 2) {
            num_mispredictions = num_mispredictions+1;
        }
        //Update Values inside of the Local history register
        if (taken_2 == 116) {
            if (local_hist_reg[new_predict] != 3) {
                local_hist_reg[new_predict] = int(local_hist_reg[new_predict] + 1);\
            }
            hist = (hist >> 1) | (1 << (n-1));
        }
        else if (taken_2 == 110) {
            if (local_hist_reg[new_predict] != 0) {
                local_hist_reg[new_predict] = int(local_hist_reg[new_predict] - 1);
            }
            hist = (hist >> 1); 
        }
    }
    //Output results to file
    cout << '\n' << num_predictions << "   ";   cout << num_mispredictions << "   ";
    double accuracy_n = (static_cast<double>(num_mispredictions) / static_cast<double>(num_predictions)) * static_cast<double>(100);
    cout << accuracy_n << '\n';
    //Print final Bimodal Info
    outfile << "COMMAND\n";
    outfile << " sim gshare " << m << n << FileName << '\n' << "OUTPUT\n";
    outfile << " number of predictions:" << '\t' << num_predictions << '\n';
    outfile << " number of mispredictions:" << '\t' << num_mispredictions << '\n';
    outfile << " misprediction rate:" << '\t' << '\t' << accuracy_n << "%\n";
    outfile << "FINAL GSHARE CONTENTS\n";
    for (int temp_print = 0; temp_print <= (m_i-1); temp_print++) {
        outfile << temp_print << '\t' << local_hist_reg[temp_print] << '\n';
    }
    predict_me.CloseFile();
    outfile.close();
}