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
    50, // n
    1.0,    // T
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    false,    // option_type
    0.05    // alpha
};

int main() {
    int N = 2000;
    int INITIAL_PRICE[] = {50,95,100,105,150};
    double SIGMAS[] = {.2,.4};
    double TIMES[] = {1,2};
    int n = OPTION_DATA.n;
    double K = OPTION_DATA.K;
    double r = OPTION_DATA.r;
    bool option_type = OPTION_DATA.option_type;
    vector<vector<double>> data(16,vector<double>(9));
    for (int p = 0; p<5;p++) {
        for(int s = 0; s < 2;s++) {
            for(int t = 0; t < 2; t++){
                double s0 = INITIAL_PRICE[p];
                double sigma = SIGMAS[s];
                double T = TIMES[t];
                cout << s0 << " " << sigma << " " << T << endl;
                int index = 5*p + 2*p*s + t;
                data[index][0] = s0;
                data[index][1] = sigma;    
                data[index][2] = T;
                cout << "here 1" << endl;
                data[index][3] = full_finite_diference(s0,OPTION_DATA.K,T,sigma,r,N,option_type);
                data[index][4] = black_scholes(r,s0,K,T,sigma,option_type);
                data[index][5] = abs(data[index][3] - data[index][4]); 
                cout << "here 2" << endl;
                data[index][6] = longstaff_schwartz(N,n,r,s0,K,T,sigma,option_type,true);
                cout << "here 3" << endl;
                data[index][7] = abs(data[index][6] - data[index][4]); 
                data[index][8] = data[index][3] - data[index][7];
            }
        } 
    }

    ofstream outputFile;
    outputFile.open("longstaff_schwartz.csv");
    string header = "S,sigma,T,diferencias finitas,black-scholes,ejercicio temprano d.f.,longstaff-schwartz,ejercicio temprano l.s., d.f. - l.s.";
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