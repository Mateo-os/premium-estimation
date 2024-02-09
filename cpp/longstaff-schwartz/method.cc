#include <bits/stdc++.h>
#include <future>
#include <iostream>
#include <chrono>
#include "helper.h"
#include "sample.h"
#include "polyRegression.h"

random_device rd;
mt19937 gen(rd());

const int P_ORDER = 2;

typedef vector<vector<double>> matrix;

double generate_negative_sample(double prev_anthi,double anthi, double sample, double dt){
    double anthi_exponential_term = anthi/(prev_anthi * exp((r - (sigma * sigma) / 2) * dt));
    return sample * exp((r - (sigma * sigma)/2) * dt)/anthi_exponential_term;
}

double calculate_polynomial(int order, vector<double> &coeffs,double x){
    double result = 0;
    for(int i = 0; i <= order ; i++){
        result += pow(x,i) * coeffs[i];
    }
    return result;
}

void generate_stock(int N,int n,double r,double s0,double T, double sigma,bool variance_reduction, vector<vector<double>> &stock){
    double dt = T/static_cast<double>(n);
    double sq_dt = sqrt(dt);
    normal_distribution<> d(0, sq_dt);
    if (variance_reduction){
        for(int i = 0; i < N/2; i++){
            stock[i][0] = s0;
            for(int j = 1; j < n+1; j++)
                stock[i][j] = generate_new_sample(stock[i][j-1],dt,sigma,r,gen, d);
        }
        for(int i = N/2; i< N; i++){
            stock[i][0] = s0;
            int anthi = (N - 1) - i;

            for(int j = 1; j < n+1; j++)
                stock[i][j] = generate_negative_sample(stock[anthi][j - 1],stock[anthi][j],stock[i][j-1],r,dt);
        }
        return;
    }
    for(int i = 0; i < N; i++){
        stock[i][0] = s0;
        for(int j = 1; j < n+1; j++)
            stock[i][j] = generate_new_sample(stock[i][j-1],dt,sigma,r,gen, d);
        
    }
    return;
}

double longstaff_schwartz(int N, int n, double r, double s0, double K,double T, double sigma,bool type, bool variance_reduction = false){
    matrix cash_flow(N,vector<double>(n+1,0));
    matrix stock(N,vector<double>(n+1,0));
    if(variance_reduction && N%2 == 1){
        throw("N must be even for variance reduction");
    }
    generate_stock(N,n,r,s0,T,sigma,variance_reduction,variance_reduction,stock);
    for(int i = 0; i < N; i++)
        cash_flow[i][n] = excercise_option(stock[i][n],K,type);

    float discount_rate = exp(-r * T/static_cast<double>(n));

    vector<int> indexes;
    vector<double> x_vector;
    vector<double> y_vector;
    vector<double> coeffs;
    PolynomialRegression<double> fitter;
    for(int j = n-1; j >= 1; j--){
        for(int i = 0;i<N;i++){
            bool condition = type ? stock[i][j] > K : K > stock[i][j];
            if(condition)
                indexes.push_back(i);
        }

        if(!indexes.size()){
            for(int i = 0; i < N; i++)
                cash_flow[i][j] = 0;
        } else{
            for (int itr = 0; itr < indexes.size(); ++itr){
                int i = indexes[itr];
                int k = j+1;
                while((k< n+1) && (cash_flow[i][k] == 0.0))
                    k++;
                double y = 0;
                if (k < n +1)
                    y = cash_flow[i][k] * pow(discount_rate,k - j);
                x_vector.push_back(stock[i][j]);
                y_vector.push_back(y);
            }
            fitter.fitIt(
                vector<double>(x_vector.begin(),x_vector.end()),
                vector<double>(y_vector.begin(),y_vector.end()),
                P_ORDER,
                coeffs
            );
            for(int itr = 0; itr < indexes.size(); ++itr){
                int i = indexes[itr];
                double excercise_value = excercise_option(stock[i][j],K,type);
                double continue_value = calculate_polynomial(P_ORDER,coeffs,stock[i][j]);
                if(continue_value < excercise_value){
                    cash_flow[i][j] = excercise_value;
                    fill(cash_flow[i].begin() + j+1,cash_flow[i].end(),0.0);
                }
            }

        }
        indexes.clear();
        x_vector.clear();
        y_vector.clear();
        
    }

    vector<double> discounted_values(N,0);
    for(int i =0; i<N;i++){
        int index = 0;
        for(int j = 1;j<n+1;j++){
            if(cash_flow[i][j] != 0){
                index = j;
                break;
            }
        }
        discounted_values[i] = cash_flow[i][index] * pow(discount_rate,index) ;
    }
    double X = mean(discounted_values);
    double excercise_value = excercise_option(s0,K,type); 
    return max(excercise_value,X);
}

int main() {
    double s0 = 36,r = 0.06, T = 1,sigma = 0.2;
    int n = 50,N = 20000;
    bool type = false;
    double POSSIBLE_STRIKES[5] = {36, 38, 40, 42, 44};
    vector<vector<double>> puts(5,vector<double>(2,0));
    for(int i = 0; i<5;i++){
        double K = POSSIBLE_STRIKES[i];
        double ls = longstaff_schwartz(N,n,r,s0,K,T,sigma,type,true);
        cout << K << ' ' << ls << endl; 
    }

    return 0;
}