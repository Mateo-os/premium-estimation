#include <bits/stdc++.h>
#include "helper.h"
#include "sample.h"

#define value 0
#define sTheta 1
#define bTheta 2

pair<double, double> broadie_glasserman_optimized(const int b, int N, double T, double s0, double K, double sigma, double r, string option_type, bool variance_reduction = false)
{
    double discount_rate = exp(-r * T / N);
    double dt = T / N;
    double val;
    double dynamic_matrix[b][N + 1][3];
    memset(dynamic_matrix, 0, sizeof(dynamic_matrix));
    vector<int> index_vector(N + 1, 0);
    dynamic_matrix[0][0][value] = s0;

    for (int j = 1; j < N + 1; j++)
    {
        dynamic_matrix[0][j][value] = generate_new_sample(dynamic_matrix[0][j - 1][value], dt, sigma, r);
    }

    int step = N;
    while (step >= 0)
    {
        int branch = index_vector[step];
        val = dynamic_matrix[branch][step][value];

        if (step == N && branch < b - 1)
        {
            dynamic_matrix[branch][step][sTheta] = excercise_option(val, K, option_type);
            dynamic_matrix[branch][step][bTheta] = excercise_option(val, K, option_type);

            double father_val = dynamic_matrix[index_vector[step - 1]][step - 1][value];
            if (branch % 2 == 0 && variance_reduction)
            {
                dynamic_matrix[branch + 1][step][value] = generate_negative_sample(father_val, val, dt, sigma, r);
            }
            else
            {
                dynamic_matrix[branch + 1][step][value] = generate_new_sample(father_val, dt, sigma, r);
            }
            index_vector[step] += 1;
        }

        else if (step == N && branch == b - 1)
        {
            dynamic_matrix[branch][step][sTheta] = excercise_option(val, K, option_type);
            dynamic_matrix[branch][step][bTheta] = excercise_option(val, K, option_type);
            index_vector[step] = -1;
            step -= 1;
        }

        else if (step < N && branch < b - 1)
        {
            vector<double> children_sThetas;
            vector<double> children_bThetas;
            for (int i = 0; i < b; i++)
            {
                children_sThetas.push_back(dynamic_matrix[i][step + 1][sTheta]);
                children_bThetas.push_back(dynamic_matrix[i][step + 1][bTheta]);
            }
            dynamic_matrix[branch][step][sTheta] = calculate_sTheta(val, children_sThetas, K, discount_rate, option_type);
            dynamic_matrix[branch][step][bTheta] = calculate_bTheta(val, children_bThetas, K, discount_rate, option_type);

            if (step > 0)
            {
                double father_val = dynamic_matrix[index_vector[step - 1]][step - 1][value];
                if (branch % 2 == 0 && variance_reduction)
                {
                    dynamic_matrix[branch + 1][step][value] = generate_negative_sample(father_val, val, dt, sigma, r);
                }
                else
                {
                    dynamic_matrix[branch + 1][step][value] = generate_new_sample(father_val, dt, sigma, r);
                }
                index_vector[step] += 1;
                double prev_val = dynamic_matrix[branch + 1][step][value];
                for (int k = step + 1; k < N; k++)
                {
                    dynamic_matrix[0][k][value] = generate_new_sample(prev_val, dt, sigma, r);
                    prev_val = dynamic_matrix[0][k][value];
                    index_vector[k] = 0;
                }
                step = N;
            }
            else
            {
                step -= 1;
            }
        }

        else if (step < N && branch == b - 1)
        {
            vector<double> children_sThetas;
            vector<double> children_bThetas;
            for (int i = 0; i < b; i++)
            {
                children_sThetas.push_back(dynamic_matrix[i][step + 1][sTheta]);
                children_bThetas.push_back(dynamic_matrix[i][step + 1][bTheta]);
            }
            dynamic_matrix[branch][step][sTheta] = calculate_sTheta(val, children_sThetas, K, discount_rate, option_type);
            dynamic_matrix[branch][step][bTheta] = calculate_bTheta(val, children_bThetas, K, discount_rate, option_type);
            index_vector[step] = -1;
            step -= 1;
        }
    }
    return make_pair(dynamic_matrix[0][0][sTheta], dynamic_matrix[0][0][bTheta]);
}
