#ifndef SAMPLE_H
#define SAMPLE_H

double generate_new_sample(double sample, double dt, double sigma, double r, mt19937 &gen, normal_distribution<> &d);

#endif // SAMPLE_H