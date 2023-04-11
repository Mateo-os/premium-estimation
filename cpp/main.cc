#include <bits/stdc++.h>
#include <chrono>
#include "helper.h"
#include "sample.h"
#include "optimizado.h"

using namespace std;

#define s0 100
#define K 100
#define sigma 0.1
#define T 1
#define r 0.05
#define b 40
#define N 4
#define n 10
#define alpha 0.05
#define now chrono::high_resolution_clock::now
const double z_alpha = ppf();

int main()
{
    vector<double> lower(n);
    vector<double> upper(n);
    string name = "OPTIMIZADO";
    cout << "-------------------" << name << "-------------------------" << endl;
    auto t2_start = now();
    for (int i = 0; i < n; i++)
    {
        auto data = broadie_glasserman_optimized(b, N, T, s0, K, sigma, r, "C", true);
        cout << i << ": " << data.first << " " << data.second << endl;
        lower[i] = data.first;
        upper[i] = data.second;
    }
    auto t2_stop = now();
    double sTheta = mean(lower);
    double bTheta = mean(upper);
    double sstd = std_v(lower);
    double bstd = std_v(upper);
    cout << "Variancia sTheta:" << sstd << ". Variancia bTheta: " << bstd << endl;
    double interval_d = max(max((double)s0 - K, 0.0), sTheta - sstd * z_alpha);
    double interval_u = bTheta + bstd * z_alpha;
    pair<double, double> interval = make_pair(interval_d, interval_u);
    cout << "Intervalo: (" << interval.first << ", " << interval.second << ")" << endl;
    cout << "Tiempo en segundos: " << (t2_stop - t2_start).count() / 1000000000.0 << "s, media: " << (t2_stop - t2_start).count() / n / 1000000000.0 << "s" << endl;

    return 0;
}