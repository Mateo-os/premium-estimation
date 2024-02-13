#include <bits/stdc++.h>
#include <future>
#include <iostream>
#include <chrono>
#include "helper.h"
#include "sample.h"



struct Stock {
    double val;
    double current;
};


random_device rd;
mt19937 gen(rd());


double tilley(int P, int Q, double s0, double K, double sigma, double r, double T, int N, int op_type, bool sharp_boundary ) {
    int R = P * Q;
    double dt = T / N;
    double discount_rate = exp(-r * dt);
    vector<int> indexes(R);
    for (int i = 0; i < R; ++i)
        indexes[i] = i;
    vector<vector<Stock>> stocks(R, vector<Stock>(N));
    vector<vector<int>> true_indicator(R, vector<int>(N));
    normal_distribution<> d(0, sqrt(dt));
    for (int path = 0; path < R; ++path) {
        double prev = s0;
        for (int step = 0; step < N; ++step) {
            prev = generate_new_sample(prev, dt, sigma, r,gen,d);
            stocks[path][step].val = prev;
        }
        stocks[path][N - 1].current = excercise_option(prev, K, op_type);
    }

    for (int step = N - 2; step >= 0; --step) {
        auto key = [&](int x) { return stocks[x][step].val; };
        std::sort(indexes.begin(), indexes.end(), [&](int a, int b) { return op_type? key(a) < key(b) : key(a) > key(b); });
        vector<int> tentative_indicator(R, -1);
        vector<double> holding_values(Q);

        for (int bundle = 0; bundle < Q; ++bundle) {
            vector<double> bundle_currents;
            for (int path = 0; path < P; ++path) {
                int path_index = indexes[bundle * P + path];
                double current_value = stocks[path_index][step + 1].current;
                bundle_currents.push_back(current_value);
            }
            double holding_value = discount_rate * std::accumulate(bundle_currents.begin(), bundle_currents.end(), 0.0) / P;
            holding_values[bundle] = holding_value;
            for (int path = 0; path < P; ++path) {
                int path_index = indexes[bundle * P + path];
                double val = stocks[path_index][step].val;
                tentative_indicator[bundle * P + path] = (excercise_option(val, K, op_type) > holding_value) ? 1 : 0;
            }
        }

        int boundary = R;
        int i = R - 1;
        int max_zero_len = 0;
        while (i >= 0) {
            int j = i;
            if (tentative_indicator[i] == 1) {
                int one_len = 0;
                while (tentative_indicator[j] == 1 && j >= 0) {
                    ++one_len;
                    --j;
                }
                if (max_zero_len < one_len) {
                    boundary = j + 1;
                }
                i = j;
            } else {
                int zero_len = 0;
                while (tentative_indicator[j] == 0 && j >= 0) {
                    ++zero_len;
                    --j;
                }
                max_zero_len = max(zero_len, max_zero_len);
            }
            i = j;
        }

        vector<int> sharp_boundary_indicator(R);
        for (int i = boundary; i < R; ++i)
            sharp_boundary_indicator[i] = 1;

        vector<int>& indicator = sharp_boundary ? sharp_boundary_indicator : tentative_indicator;

        for (int bundle = 0; bundle < Q; ++bundle) {
            double holding_value = holding_values[bundle];
            for (int path = 0; path < P; ++path) {
                int path_index = indexes[bundle * P + path];
                double val = stocks[path_index][step].val;
                stocks[path_index][step].current = indicator[bundle * P + path] == 1 ? excercise_option(val, K, op_type) : holding_value;
            }
        }

        if (std::count(indicator.begin(), indicator.end(), 1) > 0) {
            int first_one = std::distance(indicator.begin(), std::find(indicator.begin(), indicator.end(), 1));
            true_indicator[indexes[first_one]][step] = 1;
            for (int j = step + 1; j < N; ++j)
                true_indicator[indexes[first_one]][j] = 0;
        }
    }

    double estimator = 0.0;
    for (int path = 0; path < R; ++path) {
        int step = std::find(true_indicator[path].begin(), true_indicator[path].end(), 1) - true_indicator[path].begin();
        step = (step == N) ? N - 1 : step;
        double val = excercise_option(stocks[path][step].val, K, op_type);
        estimator += exp(-r * (step + 1) * dt) * val;
    }
    return estimator / R;
}
