#include <bits/stdc++.h>
#include <future>
#include "helper.h"
#include "sample.h"

#define value 0
#define sTheta 1
#define bTheta 2

typedef vector<vector<vector<double>>> matrix;


random_device rd;
mt19937 gen(rd());

void compute_parallel(matrix &dynamic_matrix, const int b, int N, double T, double s0, double K, double sigma, double r, bool option_type, bool variance_reduction = false)
{
    double dt = T / N;
    double discount_rate = exp(-r * T / N);
    double sq_dt = sqrt(dt);

    normal_distribution<> d(0, sq_dt);

    double val;
    
    vector<int> index_vector(N, 0);

    int step = N - 1;
    while (step >= 0)
    {
        int branch = index_vector[step];
        val = dynamic_matrix[value][step][branch];

        if (step == N-1 && branch < b - 1)
        {
         
            dynamic_matrix[sTheta][step][branch] = excercise_option(val, K, option_type);
            dynamic_matrix[bTheta][step][branch] = excercise_option(val, K, option_type);

            int father_branch = index_vector[step - 1];
            double father_val = dynamic_matrix[value][step - 1][father_branch];

            if (branch % 2 == 0 && variance_reduction)
            {
                dynamic_matrix[value][step][branch + 1] = generate_negative_sample(father_val, val, dt, sigma, r);
            }
            else
            {
                dynamic_matrix[value][step][branch + 1] = generate_new_sample(father_val, dt, sigma, r, gen, d);
            }
            index_vector[step] += 1;
        }
        else if (step == N - 1 && branch == b - 1)
        {
            dynamic_matrix[sTheta][step][branch] = excercise_option(val, K, option_type);
            dynamic_matrix[bTheta][step][branch] = excercise_option(val, K, option_type);
            index_vector[step] = 0;
            step -= 1;
        }

        else if (step < N - 1 && branch < b - 1)
        {
            vector<double> &children_sThetas = dynamic_matrix[sTheta][step + 1];
            vector<double> &children_bThetas = dynamic_matrix[bTheta][step + 1];
            dynamic_matrix[sTheta][step][branch] = calculate_sTheta(val, children_sThetas, K, discount_rate, option_type);
            dynamic_matrix[bTheta][step][branch] = calculate_bTheta(val, children_bThetas, K, discount_rate, option_type);

            if (step > 0)
            {

                int father_branch = index_vector[step - 1];
                assert(father_branch >= 0 && father_branch < b);
                double father_val = dynamic_matrix[value][step - 1][father_branch];
                if (branch % 2 == 0 && variance_reduction)
                    dynamic_matrix[value][step][branch + 1] = generate_negative_sample(father_val, val, dt, sigma, r);
                else
                    dynamic_matrix[value][step][branch + 1] = generate_new_sample(father_val, dt, sigma, r, gen, d);
                index_vector[step] += 1;
                double prev_val = dynamic_matrix[value][step][branch + 1];
                for (int k = step + 1; k < N; k++)
                {
                    dynamic_matrix[value][k][0] = generate_new_sample(prev_val, dt, sigma, r, gen, d);
                    prev_val = dynamic_matrix[value][k][0];
                    index_vector[k] = 0;
                }
                step = N - 1;
            }
            else
                step -= 1;
        }
        else if (step < N - 1 && branch == b - 1)
        {
            vector<double> &children_sThetas = dynamic_matrix[sTheta][step + 1];
            vector<double> &children_bThetas = dynamic_matrix[bTheta][step + 1];
            dynamic_matrix[sTheta][step][branch] = calculate_sTheta(val, children_sThetas, K, discount_rate, option_type);
            dynamic_matrix[bTheta][step][branch] = calculate_bTheta(val, children_bThetas, K, discount_rate, option_type);
            index_vector[step] = -1;
            step -= 1;
        }
    }
}

pair<double, double> broadie_glasserman_parallel(const int b, int N, double T, double s0, double K, double sigma, double r, bool option_type, bool variance_reduction = false)
{
    double dt = T / N;
    double sq_dt = sqrt(dt);
    
    normal_distribution<> d(0, sq_dt);

    // type deph branch
    vector<matrix> dynamic_matrix(b, matrix(3, vector<vector<double>>(N, vector<double>(b, 0))));

    vector<future<void>> futures;

    for (int i = 0; i < b; i++)
    {
        double last_s1;
        if ((i % 2 == 1) && variance_reduction){
            last_s1 = dynamic_matrix[i-1][value][0][0];
            dynamic_matrix[i][value][0][0] = generate_negative_sample(s0,last_s1,dt,sigma,r);
        }
        else
            dynamic_matrix[i][value][0][0] = generate_new_sample(s0,dt,sigma,r,gen,d);
        double prev_val = dynamic_matrix[i][value][0][0];
        for(int j = 1; j < N; j++){
            dynamic_matrix[i][value][j][0] = generate_new_sample(prev_val,dt,sigma,r,gen,d);
            prev_val = dynamic_matrix[i][value][j][0];
        }
        futures.push_back(async(launch::async, compute_parallel, ref(dynamic_matrix[i]), b, N, T, s0, K, sigma, r, option_type, variance_reduction));
    }

    for (auto &f : futures)
    {
        f.get();
    }

    vector<double> children_sThetas;
    vector<double> children_bThetas;

    for (int i = 0; i < b; i++)
    {
        children_sThetas.push_back(dynamic_matrix[i][sTheta][0][0]);
        children_bThetas.push_back(dynamic_matrix[i][bTheta][0][0]);
    }
    
    double discount_rate = exp(-r * T / N);
    
    pair<double, double> res = make_pair(
        calculate_sTheta(s0, children_sThetas, K, discount_rate, option_type),
        calculate_bTheta(s0, children_bThetas, K, discount_rate, option_type));

    return res;
}
