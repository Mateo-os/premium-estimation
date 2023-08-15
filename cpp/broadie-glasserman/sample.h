#ifndef SAMPLE_H
#define SAMPLE_H

double generate_new_sample(double sample, double dt, double sigma, double r, mt19937 &gen, normal_distribution<> &d);

double generate_negative_sample(double prev_sample, double sample, double dt, double sigma, double r);

#endif // SAMPLE_H