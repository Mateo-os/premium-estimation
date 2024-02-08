#ifndef HELPER_H
#define HELPER_H

#include <bits/stdc++.h>
#include <boost/math/distributions/normal.hpp>
using namespace std;

double ppf(double alpha = 0.05, int n = 10);

double mean(vector<double> &v);

double std_v(vector<double> &v);

double excercise_option(double val, double K, bool op_type);

#endif // HELPER_H