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
    360, // n
    1.0,    // T
    100.0,  // K
    0.05,   // r
    0.1,    // sigma
    false,    // option_type
    0.05    // alpha
};

int main() {
    int STOCK_STEPS = 2000;
    int TRAYECTORIES = 20000;
    int INITIAL_PRICE[] = {81,95,100,105,119};
    int POSIBBLE_Q[] = {100,400,800};
    double SIGMAS[] = {0.2,0.4};
    double TIMES[] = {1,2};
    int n = OPTION_DATA.n;
    double K = OPTION_DATA.K;
    double r = OPTION_DATA.r;
    bool option_type = OPTION_DATA.option_type;
    vector<vector<vector<double>>> data(2, vector<vector<double>>(30,vector<double>(10)));
    int index;
    int sum;
    for(int t = 0; t < 2; t++){
        index = 0;
        for(int s = 0; s < 2;s++) {
            double sigma = SIGMAS[s];
            double T = TIMES[t];
            cout << " D.F.  " << endl;
            vector<vector<double>> grid = explicit_finite_difference(K,T,sigma,r,STOCK_STEPS,option_type,true); 
            for (int p = 0; p<5;p++) {
                double s0 = INITIAL_PRICE[p];
                for(int q = 0; q<3;q++){
                    int Q = POSIBBLE_Q[q];
                    int P = TRAYECTORIES/Q;
                    data[t][index][0] = s0;
                    data[t][index][1] = sigma;    
                    data[t][index][2] = P; 
                    data[t][index][3] = Q;
                    data[t][index][4] = estimate_option_price(s0,K,T,grid);
                    data[t][index][5] = black_scholes(r,s0,K,T,sigma,option_type);
                    data[t][index][6] = data[t][index][4] - data[t][index][5]; 
                    data[t][index][7] = tilley(P,Q,s0,K,sigma,r,T,n*T,option_type,true);
                    data[t][index][8] = data[t][index][7] - data[t][index][5]; 
                    data[t][index][9] = data[t][index][6] - data[t][index][8];
                    index++;
                }
            }
        } 
    }

    ofstream outputFile;
    string header = "S,siggma,P,Q,diferenciasfinitas,blackscholes,ejerciciotempranodf,tilley,ejerciciotempranotilley,dftilley";
    for(int t = 0; t < 2; t++){
        int T = TIMES[t];
        string filename = "tilley_T" + to_string(T) + ".csv";
        outputFile.open(filename);
        outputFile << header << endl;
        for(int i = 0; i<30;i++){
            for(int j = 0;j<9;j++){
                outputFile << data[t][i][j] << ',';
            }
            outputFile << data[t][i][9] << endl;
        }
        outputFile.close();
    }
    return 0;
}