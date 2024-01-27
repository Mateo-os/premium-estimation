#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <chrono>

using namespace std;

double generate_new_sample(double sample, double dt, double sigma, double r, mt19937 &gen, normal_distribution<> &d)
{
    double Z = d(gen);
    double new_sample = sample * exp((r - (sigma * sigma) / 2) * dt + sigma * Z);
    return new_sample;
}
