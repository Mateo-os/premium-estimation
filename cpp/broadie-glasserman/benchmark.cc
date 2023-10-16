#include <bits/stdc++.h>
#include <chrono>
#include "helper.h"
#include "sample.h"
#include "optimizado.h"
#include "parallel.h"

namespace math = boost::math;

using namespace std;

// Constants
const int DATA_LEN = 5;
const int POSSIBLE_B[] = {4, 8, 10,16, 20 };
                        //32,50,80,100,150};
const int POSSIBLE_N[] = {10, 30, 50, 70, 80};
                        //,100,200,300,400,500};

// Option data
struct OptionData {
    int N;
    double T;
    double s0;
    double K;
    double r;
    double sigma;
    bool option_type;
    bool variance_reduction;
    double alpha;
};

//test 
OptionData OPTION_DATA = {
    4,      // N
    1.0,    // T
    100.0,  // s0
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    true,    // option_type
    true,   // variance_reduction
    0.05    // alpha
};

vector<double> broadie_glasserman_full(int iterations, int branches, bool parallel) {
    int N = OPTION_DATA.N;
    double T = OPTION_DATA.T;
    double s0 = OPTION_DATA.s0;
    double K = OPTION_DATA.K;
    double r = OPTION_DATA.r;
    double sigma = OPTION_DATA.sigma;
    bool option_type = OPTION_DATA.option_type;
    bool variance_reduction = OPTION_DATA.variance_reduction;
    double z_alpha = ppf(OPTION_DATA.alpha,iterations);
    vector<double> lower(iterations);
    vector<double> upper(iterations);

    for (int i = 0; i < iterations; ++i) {
        pair <double,double> data = parallel ? 
            broadie_glasserman_parallel(branches,N,T,s0,K,sigma,r,option_type,variance_reduction) 
            : broadie_glasserman_optimized(branches,N,T,s0,K,sigma,r,option_type,variance_reduction);
        lower[i] = data.first;
        upper[i] = data.second;
    }

    double sTheta = mean(lower);
    double bTheta = mean(upper);
    double sstd = std_v(lower) ;
    double bstd = std_v(upper);

    vector<double> interval = {
        max(max(s0 - K, 0.0), sTheta - sstd * z_alpha),
        bTheta + bstd * z_alpha
    };

    vector<double> results = {sTheta, sstd, bTheta, bstd, interval[0], interval[1]};

    return results;
}

int main() {
    vector<vector<vector<vector<double>>>> data(2, vector<vector<vector<double>>>(2, vector<vector<double>>(DATA_LEN, vector<double>(8))));
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
        int n = 50;
        int b = 16;
        if(fixed_val)
            copy(begin(POSSIBLE_B), end(POSSIBLE_B), begin(iterator));
        else
            copy(begin(POSSIBLE_N), end(POSSIBLE_N), begin(iterator));

        for (int i = 0; i < DATA_LEN; ++i) {
            int val = iterator[i];
            if (fixed_val) 
                b = val; 
            else 
                n = val;
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
            data[parallel][fixed_val][i][7] = chrono::duration_cast<chrono::microseconds>(t_end - t_start).count() / 1e6;
        }

        ofstream outputFile;
        string fixd_val = fixed_val ? "n" : "b";
        string non_fixed_val = fixed_val ? "b" : "n";
        string csv_name = parallel ? "" : "non_";
        csv_name = csv_name  + "parallel_fixed_" + fixd_val + ".csv";
        outputFile.open(csv_name);

        string header = non_fixed_val + ",lower estimator,lower estimator variance,upper estimator,upper estimator variance,lower bound,upper bound,elapsed time";
        outputFile << header << endl;

        for (int i = 0; i < DATA_LEN; ++i) {
            for (int j = 0; j < 8; ++j) {
                outputFile << data[parallel][fixed_val][i][j] << ",";
            }
            outputFile << endl;
        }

        outputFile.close();
    }
    return 0;
}