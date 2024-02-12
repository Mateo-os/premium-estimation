#include <iostream>
#include <cmath>
#include <vector>
#include "helper.h" 

using namespace std;

double estimate_option_price(double s0, double K, double T, const vector<vector<double>>& grid) {
    int S_steps = grid.size();
    int T_steps = grid[0].size();
    double ds = 2 * K / S_steps;
    double dt = T / T_steps;
    int index = static_cast<int>(s0 / ds);

    if (index * ds == s0) {
        return grid[index][T_steps - 1];
    }

    double v1 = grid[(index + 1) / ds][T_steps - 1];
    double v2 = grid[index][T_steps - 1];

    double a2 = ((index + 1) * ds - s0) * dt;
    double a1 = (s0 - index * ds) * dt;

    return (v1 * a1 + v2 * a2) / (a1 + a2);
}


vector<vector<double>> explicit_finite_difference(double K, double T, double sigma, double r, int asset_steps, bool op_type, bool early_exercise=false) {
    double ds = 2 * K / asset_steps;
    double dt = 0.9 / pow(sigma * asset_steps, 2);
    int time_steps = static_cast<int>(T / dt) + 1;
    cout << "TIME STEPS: " << time_steps << endl;
    cout << "dS: " << ds << endl;
    dt = T / time_steps;
    cout << "dt: " << dt << endl;

    vector<double> asset_array(asset_steps);
    vector<vector<double>> grid(asset_steps, vector<double>(time_steps + 1, 0.0));
    vector<double> payoff(asset_steps);

    for (int i = 0; i < asset_steps; ++i) {
        double val = i * ds;
        asset_array[i] = val;
        grid[i][0] = excercise_option(val, K, op_type); // Exercise the option at time T
        payoff[i] = grid[i][0];
    }

    for (int j = 1; j <= time_steps; ++j) {
        for (int i = 1; i < asset_steps - 1; ++i) {
            double s = asset_array[i];
            
            double delta = (grid[i + 1][j - 1] - grid[i - 1][j - 1]) / (2 * ds);
            double gamma = (grid[i + 1][j - 1] - 2 * grid[i][j - 1] + grid[i - 1][j - 1]) / pow(ds, 2);
            double theta = -0.5 * pow(sigma, 2) * pow(s, 2) * gamma - r * s * delta + r * grid[i][j - 1];

            grid[i][j] = grid[i][j - 1] - dt * theta;
        }

        grid[0][j] = grid[0][j - 1] * (1 - r) * dt;
        grid[asset_steps - 1][j] = 2 * grid[asset_steps - 2][j] - grid[asset_steps - 3][j];

        if (early_exercise) {
            for (int i = 0; i < asset_steps; ++i) {
                grid[i][j] = max(grid[i][j], payoff[i]);
            }
        }
    }

    return grid;
}

int main() {
    // Example usage
    double K = 100;
    double T = 1;
    double sigma = 0.1;
    double r = 0.05;
    int asset_steps = 1000;
    int op_type = false;
    bool early_exercise = true;

    vector<vector<double>> result = explicit_finite_difference(K, T, sigma, r, asset_steps, op_type, early_exercise);
    double price = estimate_option_price(100,K,T,result);
    cout << "Price: " << price << endl;
    cout << "Black Scholes: " << black_scholes(r,100,K,T,sigma,false) << endl;
    return 0;
}