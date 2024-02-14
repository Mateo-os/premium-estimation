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
    2000, // n
    1.0,    // T
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    true,    // option_type
    0.05    // alpha
};

int main() {
    int STOCK_STEPS = 2000;
    int TRAYECTORIES = 10000;
    int INITIAL_PRICE[] = {50,95,100,105,150};
    double SIGMAS[] = {0.2,0.4};
    double TIMES[] = {1,2};
    int n = OPTION_DATA.n;
    double K = OPTION_DATA.K;
    double r = OPTION_DATA.r;
    bool option_type = OPTION_DATA.option_type;
    int index = 0;
    vector<vector<double>> data(20,vector<double>(9));
    for (int p = 0; p<5;p++) {
        for(int s = 0; s < 2;s++) {
            for(int t = 0; t < 2; t++){
                double s0 = INITIAL_PRICE[p];
                double sigma = SIGMAS[s];
                double T = TIMES[t];
                data[index][0] = s0;
                data[index][1] = sigma;    
                data[index][2] = T;
                data[index][3] = full_finite_diference(s0,OPTION_DATA.K,T,sigma,r,STOCK_STEPS,option_type,true);
                data[index][4] = black_scholes(r,s0,K,T,sigma,option_type);
                data[index][5] = data[index][3] - data[index][4]; 
                data[index][6] = longstaff_schwartz(TRAYECTORIES,n,r,s0,K,T,sigma,option_type,true);
                data[index][7] = data[index][6] - data[index][4]; 
                data[index][8] = data[index][3] - data[index][6];
                index++;
            }
        } 
    }

    ofstream outputFile;
    outputFile.open("longstaff_schwartz.csv");
    string header = "S,siggma,T,diferenciasfinitas,blackscholes,ejerciciotempranodf,longstaff,ejerciciotempranols,dfls";
    outputFile << header << endl;
    for(int i = 0; i<20;i++){
        for(int j = 0;j<8;j++){
            outputFile << data[i][j] << ',';
        }
        outputFile << data[i][8] << endl;
    }
    outputFile.close();
    return 0;
}