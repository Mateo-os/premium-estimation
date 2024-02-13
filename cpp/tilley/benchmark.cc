#include <bits/stdc++.h>
#include <chrono>
#include "helper.h"
#include "sample.h"
#include "method.h"
#include "fdif.h"

using namespace std;


// Option data
struct OptionData {
    int n;
    double T;
    double K;
    double r;
    double sigma;
    bool option_type;
    double alpha;
};

OptionData OPTION_DATA = {
    5000, // n
    1.0,    // T
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    false,    // option_type
    0.05    // alpha
};

int main() {
    int STOCK_STEPS = OPTION_DATA.n;
    int TRAYECTORIES = 10000;
    int INITIAL_PRICE[] = {50,95,100,105,150};
    int POSIBBLE_Q[] = {100,200,400};
    double SIGMAS[] = {0.2,0.4};
    double TIMES[] = {1,2};
    int TABLE_SIZE = (sizeof(INITIAL_PRICE) * sizeof(POSIBBLE_Q) * sizeof(TIMES) * sizeof(SIGMAS)) / (pow(sizeof(int),2) * pow(sizeof(double),2));
    int n = OPTION_DATA.n;
    double K = OPTION_DATA.K;
    double r = OPTION_DATA.r;
    bool option_type = OPTION_DATA.option_type;
    int index = 0;
    vector<vector<double>> data(60,vector<double>(11));
    for (int p = 0; p<5;p++) {
        for(int s = 0; s < 2;s++) {
            for(int t = 0; t < 2; t++){
                double s0 = INITIAL_PRICE[p];
                double sigma = SIGMAS[s];
                double T = TIMES[t];
                double fdiff = full_finite_diference(s0,OPTION_DATA.K,T,sigma,r,STOCK_STEPS,option_type,true);
                double b_s = black_scholes(r,s0,K,T,sigma,option_type);
                for(int q = 0; q<3;q++){

                    int Q = POSIBBLE_Q[q];
                    int P = TRAYECTORIES/Q;
                    data[index][0] = s0;
                    data[index][1] = sigma;    
                    data[index][2] = T;
                    data[index][3] = P; 
                    data[index][4] = Q;
                    data[index][5] = fdiff;
                    data[index][6] = b_s;
                    data[index][7] = data[index][5] - data[index][6]; 
                    data[index][8] = tilley(P,Q,s0,K,sigma,r,T,n,option_type,true);
                    data[index][9] = data[index][8] - data[index][6]; 
                    data[index][10] = data[index][9] - data[index][7];
                    index++;
                }
            }
        } 
    }

    ofstream outputFile;
    outputFile.open("tilley.csv");
    string header = "S,sigma,T,P,Q, diferencias finitas,black-scholes,ejercicio temprano d.f.,tilley,ejercicio temprano tilley, d.f. - tilley";
    outputFile << header << endl;
    for(int i = 0; i<TABLE_SIZE;i++){
        for(int j = 0;j<10;j++){
            outputFile << data[i][j] << ',';
        }
        outputFile << data[i][10] << endl;
    }
    outputFile.close();
    return 0;
}