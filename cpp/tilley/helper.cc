#include <bits/stdc++.h>
#include <boost/math/distributions/normal.hpp>

using namespace std;

double ppf(double alpha = 0.05, int n = 10)
{
    boost::math::normal_distribution<> myNormal(0, 1);
    return boost::math::quantile(myNormal, 1.0 - alpha / 2.0) / sqrt(n);
}

double mean(vector<double> &v)
{
    return accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double std_v(vector<double> &v)
{
    double sum = 0;
    double m = mean(v);
    for (long unsigned int i = 0; i < v.size(); i++)
    {
        sum += pow(v[i] - m, 2);
    }
    return sqrt(sum / (v.size() - 1));
}

double excercise_option(double val, double K, bool op_type)
{
    if (op_type)
    {
        return max(val - K, 0.0);
    }
    else
    {
        return max(K - val, 0.0);
    }
}