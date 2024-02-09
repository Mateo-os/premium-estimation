#include <bits/stdc++.h>
#include <chrono>
#include "helper.h"
#include "sample.h"
#include "method.cc"

namespace math = boost::math;

using namespace std;

// Constants
const int POSSIBLE_N[] = {1000, 3000, 5000, 7000, 8000 ,20000,30000,40000,50000,60000,70000,80000,90000,100000};
const int DATA_LEN = POSSIBLE_N.size();

// Option data
struct OptionData {
    int N;
    double T;
    double s0;
    double K;
    double r;
    double sigma;
    bool option_type;
    double alpha;
};


OptionData OPTION_DATA = {
    4,      // N
    1.0,    // T
    100.0,  // s0
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    false,    // option_type
    0.05    // alpha
};

int main() {
    vector<vector<vector<vector<double>>>> data(2, vector<vector<vector<double>>>(2, vector<vector<double>>(DATA_LEN, vector<double>(9))));
    int iterator[DATA_LEN];
    int parallel = 1;
    string test_prompt = parallel ? "Parallel test" : "Non parallel test";
    cout << test_prompt << endl;
    for(int fixed_val = 0; fixed_val < 2; fixed_val++){
        /*FIXED_VAL:
        0 -> fixed b test
        1 -> fixed n test
        */
        test_prompt = fixed_val ? "Fixed n test" : "Fixed b test";
        cout << test_prompt << endl;
        int N = 80;
        int B = 16;
        if(fixed_val)
            copy(begin(POSSIBLE_B), end(POSSIBLE_B), begin(iterator));
        else
            copy(begin(POSSIBLE_N), end(POSSIBLE_N), begin(iterator));

        for (int i = 0; i < DATA_LEN; ++i) {
            int val = iterator[i];
            int n,b;
            if (fixed_val) 
                n = N,b = val; 
            else 
                n = val, b = B;
            test_prompt = fixed_val ? "Branches " : "Iterations: ";
            cout << test_prompt << val << endl;
            chrono::high_resolution_clock::time_point t_start = chrono::high_resolution_clock::now();
            vector<double> res = broadie_glasserman_full(n, b, parallel);
            chrono::high_resolution_clock::time_point t_end = chrono::high_resolution_clock::now();
            data[parallel][fixed_val][i][0] = val;
            data[parallel][fixed_val][i][1] = res[0];
            data[parallel][fixed_val][i][2] = res[1];
            data[parallel][fixed_val][i][3] = res[2];
            data[parallel][fixed_val][i][4] = res[3];
            data[parallel][fixed_val][i][5] = res[4];
            data[parallel][fixed_val][i][6] = res[5];
            data[parallel][fixed_val][i][7] = res[5] - res[4];
            data[parallel][fixed_val][i][8] = chrono::duration_cast<chrono::microseconds>(t_end - t_start).count() / 1e6;
        }

        ofstream outputFile;
        string fixd_val = fixed_val ? "n" : "b";
        string non_fixed_val = fixed_val ? "b" : "n";
        string csv_name = parallel ? "" : "non_";
        csv_name = csv_name  + "parallel_fixed_" + fixd_val + ".csv";
        outputFile.open(csv_name);

        string header = non_fixed_val + ",lower estimator,lower estimator variance,upper estimator,upper estimator variance,lower bound,upper bound,interval size,elapsed time";
        outputFile << header << endl;

        for (int i = 0; i < DATA_LEN; ++i) {
            for (int j = 0; j < 8; ++j) {
                outputFile << data[parallel][fixed_val][i][j] << ",";
            }
            outputFile << data[parallel][fixed_val][i][8] << endl;
        }

        outputFile.close();
    }
    return 0;
}