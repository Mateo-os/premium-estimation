#include <bits/stdc++.h>
#include <future>
#include <iostream>
#include "sample.h"
#include "helper.h"
random_device rd;
mt19937 gen(rd());

typedef vector<vector<vector<double>>> matrix;

void generate_stock(int N,int n,double r,double s0,double T, double sigma, vector &stock){
    double dt = T/n;
    double sq_dt = sqrt(dt);
    for(int i = 0; i < N; i++){
        stock[i][0] = s0;
        for(int j = 1; j < n+1; j++)
            stock[i][j] = generate_new_sample(stock[i][j-1],dt,sigma,r,gen, d);
        
    }
    return;
}
float longstaff_schwartz(int N, int n, double r, double s0, double K,double T, double sigma,bool type){
    matrix cash_flow(N,vector<double>(n+1,0));
    matrix stock(N,vector<double>(n+1,0));
    generate_stock(N,n,r,s0,T,sigma,matrix)
    for(int i = 0; i < N; i++)
        cash_flow[i][n] = excercise_option(stock[i][n],K,type);

    float discount_rate = exp(-r * T/n);

    set<int> indexes;
    set<double> x_vector;
    set<double> y_vector;
    set<int>::iterator itr;
    for(int j = n-1; i >= 0; j--){
        for(int i = 0;i<N;i++){
            if( K > stock[i][j])
                indexes.insert(i);
        }

        if(indexes.empty()){
            for(int i = 0; i < N; i++)
                cash_flow[i][j] = 0;
        } else{
            for (itr = indexes.begin(); itr != indexes.end(); ++itr){
                int i = *itr;
                int k = j+1;
                while(k< n+1 && cash_flow[i][k] == 0)
                    k++;
                y = (k < n +1) ? cash_flow[i][k] * pow(discount_rate,k - j) : 0;
                x_vector.insert(stock[i][j]);
                y_vector.insert(y);
            }
        }
        //curve_fit(x_vector,y_vector);
        for()
        indexes.clear();
        x_vector.clear();
        y_vector.clear();
        
    }
    return 0.0;
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}