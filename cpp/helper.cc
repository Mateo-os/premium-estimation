#include <bits/stdc++.h>
#include <boost/math/distributions/normal.hpp>

using namespace std;

double ppf(double alpha = 0.05, int n = 10)
{
    boost::math::normal_distribution<> myNormal(0, 1);
    return boost::math::quantile(myNormal, 1 - alpha / 2) / sqrt(n);
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

double excercise_option(double val, double K, string op_type)
{
    if (op_type == "C")
    {
        return max(val - K, 0.0);
    }
    else
    {
        return max(K - val, 0.0);
    }
}

double calculate_bTheta(double val, vector<double> &children_thetas, double K, double discount_rate, string op_type)
{
    double excercise_value = excercise_option(val, K, op_type);
    double holding_value = mean(children_thetas) * discount_rate;
    return max(excercise_value, holding_value);
}

double calculate_sTheta(double val, vector<double> &children_thetas, double K, double discount_rate, string op_type)
{
    double excercise_value = excercise_option(val, K, op_type);
    double children_sum = accumulate(children_thetas.begin(), children_thetas.end(), 0.0);
    vector<double> estimated_payoffs(children_thetas.size());
    for (long unsigned int i = 0; i < children_thetas.size(); i++)
    {
        double associated_value = ((children_sum - children_thetas[i]) / (children_thetas.size() - 1)) * discount_rate;
        estimated_payoffs[i] = (excercise_value < associated_value) ? children_thetas[i] * discount_rate : excercise_value;
    }
    return mean(estimated_payoffs);
}