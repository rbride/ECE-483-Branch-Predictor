#include "bi_modal.h"
using namespace std;

void bi_modal() {
    //Initialize and Fill The Bi-Model Array
    //vector<int> bi_modal(64, 2);
    string FileName, OutFileName;
    cout << "File Name: ";  cin >> FileName;
    cout << "\nOutput File Name: "; cin >> OutFileName;
    ofstream outfile;
    outfile.open(OutFileName);
    FileObj predict_me(FileName);
    predict_me.OpenFile();
    int m;
    cout << "\nLocal History Register Size (M): "; cin >> m;
    m = static_cast<int>(pow(2, m));
    vector<int> bi_modal(m, 2);
    int num_predictions = 0;
    int num_mispredictions = 0;
    while (!predict_me.EndOfFile()) {
        int index = predict_me.Parse_Index();
        //Shift Right twice to get rid of trailing 0's
        index = index >> 2;
        //get last 6 bits and use that as index for model
        int model_index = index & (m -1);
        int taken = predict_me.taken_not_taken;

        //Determine if Prediction is Correct or not
        if ((bi_modal[model_index] < 2) && (taken != 110)) {
            num_mispredictions++;
        }
        else if ((bi_modal[model_index] >= 2) && (taken != 116)) {
            num_mispredictions++;
        }
        num_predictions++;

        //Update Bimodel   Note: 110 = 'n' 116 = 't'
        if (taken == 116) {
            if (bi_modal[model_index] != 3) {
                bi_modal[model_index] = int(bi_modal[model_index] + 1);
            }
        }
        else if (taken == 110) {
            if (bi_modal[model_index] != 0) {
                bi_modal[model_index] = int(bi_modal[model_index] - 1);
            }
        }

        if (num_predictions % 100000 == 0) {
            cout << num_predictions << ' ';
        }
    }
    cout << '\n' << num_predictions << "   ";   cout << num_mispredictions << "   ";
    double accuracy = (static_cast<double>(num_mispredictions) / static_cast<double>(num_predictions)) * static_cast<double>(100);
    cout << accuracy << '\n';
    //Print final Bimodal Info
    outfile << "COMMAND\n";
    outfile << " sim bimodal " << m << FileName << '\n' << "OUTPUT\n";
    outfile << " number of predictions:" << '\t' << num_predictions << '\n';
    outfile << " number of mispredictions:" << '\t' << num_mispredictions << '\n';
    outfile << " misprediction rate:" << '\t' << '\t' << accuracy << "%\n";
    outfile << "FINAL BIMODAL CONTENTS\n";
    for (int temp_print = 0; temp_print <= (m-1); temp_print++) {
        outfile << temp_print << '\t' << bi_modal[temp_print] << '\n';
    }
    predict_me.CloseFile();
    outfile.close();
}