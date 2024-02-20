#ifndef FDIF
#define FDIF


vector<vector<double>> explicit_finite_difference(double K, double T, double sigma, double r, int asset_steps, bool op_type, bool early_exercise=true);

double estimate_option_price(double s0, double K, double T, const vector<vector<double>>& grid);

double full_finite_diference(double S0,double K, double T, double sigma, double r, int asset_steps, bool op_type, bool early_exercise=true);

#endif 