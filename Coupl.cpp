#include "Coupl.h"
#include <string>

// computes up to 3rd order nonlinear permittivity
Coupl::Coupl(size_t& nHarm, std::complex<double>& a, std::complex<double>& b, arma::vec& normE)
{
    D = arma::eye<arma::cx_mat>(nHarm,nHarm);
    N = arma::eye<arma::cx_mat>(nHarm,nHarm);
    double e[nHarm];
    for(size_t ih = 0; ih<nHarm; ih++)
    {
        e[ih] = std::abs(normE(ih));
    }
    switch(nHarm)
    {
    case 1:
        D(0,0) = a + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1);
        break;
    case 2:
        D(0,0) = 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1) + b * pow(e[1], 0.2e1) / 0.2e1 - b * e[0] * e[1] / 0.2e1 + a;
        D(1,0) = b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(0,1) = b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(1,1) = b * pow(e[0], 0.2e1) / 0.2e1 + a + 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1);
        break;
    case 3:
        D(0,0) = -b * e[1] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 - b * e[0] * e[1] / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1) + b * pow(e[1], 0.2e1) / 0.2e1 + a;
        D(1,0) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(2,0) = -b * e[0] * e[1] / 0.2e1 + b * e[0] * e[2] + b * pow(e[1], 0.2e1) / 0.4e1;
        D(0,1) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(1,1) = 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1) + a + b * e[0] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1;
        D(2,1) = b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2];
        D(0,2) = -b * e[0] * e[1] / 0.2e1 + b * e[0] * e[2] + b * pow(e[1], 0.2e1) / 0.4e1;
        D(1,2) = b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2];
        D(2,2) = b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[2], 0.2e1) + a;
        break;
    case 4:
        D(0,0) = -b * e[1] * e[2] / 0.2e1 - b * e[2] * e[3] / 0.2e1 + a - b * e[0] * e[1] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1);
        D(1,0) = b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] - b * e[0] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[1] * e[3] / 0.2e1;
        D(2,0) = b * e[1] * e[3] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[3] / 0.2e1 + b * e[0] * e[2] - b * e[0] * e[1] / 0.2e1;
        D(3,0) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[3] - b * pow(e[1], 0.2e1) / 0.4e1;
        D(0,1) = b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] - b * e[0] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[1] * e[3] / 0.2e1;
        D(1,1) = 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1) + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * e[0] * e[2] / 0.2e1 + a + b * pow(e[0], 0.2e1) / 0.2e1 - b * e[0] * e[3] / 0.2e1;
        D(2,1) = b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[3] / 0.2e1;
        D(3,1) = b * e[1] * e[3] + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1;
        D(0,2) = b * e[1] * e[3] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[3] / 0.2e1 + b * e[0] * e[2] - b * e[0] * e[1] / 0.2e1;
        D(1,2) = b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[0] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[2];
        D(2,2) = a + b * e[1] * e[3] / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[2], 0.2e1) + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1;
        D(3,2) = b * e[0] * e[1] / 0.2e1 + b * e[2] * e[3] - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1;
        D(0,3) = b * e[0] * e[3] - b * pow(e[1], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1;
        D(1,3) = -b * e[0] * e[1] / 0.2e1 + b * e[1] * e[3] + b * e[0] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1;
        D(2,3) = b * e[0] * e[1] / 0.2e1 + b * e[2] * e[3] - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1;
        D(3,3) = b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + a + b * pow(e[2], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[3], 0.2e1);
        break;
    case 5:
        D(0,0) = -b * e[1] * e[2] / 0.2e1 - b * e[2] * e[3] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + a + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 - b * e[3] * e[4] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1);
        D(1,0) = b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 - b * e[2] * e[4] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[1] - b * e[1] * e[3] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(2,0) = b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] - b * e[0] * e[1] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[1] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1;
        D(3,0) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[0] * e[3] + b * e[1] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1;
        D(4,0) = -b * e[1] * e[2] / 0.2e1 + b * e[0] * e[4] - b * e[0] * e[3] / 0.2e1 + b * e[1] * e[3] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1;
        D(0,1) = b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 - b * e[2] * e[4] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[1] - b * e[1] * e[3] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(1,1) = b * pow(e[2], 0.2e1) / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + a + 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1) - b * e[1] * e[4] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1;
        D(2,1) = b * e[2] * e[3] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(3,1) = b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[2] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[4] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[1] * e[3];
        D(4,1) = -b * e[0] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[3] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 + b * e[1] * e[4];
        D(0,2) = b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] - b * e[0] * e[1] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[1] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1;
        D(1,2) = b * e[2] * e[3] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] - b * pow(e[0], 0.2e1) / 0.4e1;
        D(2,2) = a + 0.3e1 / 0.4e1 * b * pow(e[2], 0.2e1) + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(3,2) = b * e[1] * e[4] / 0.2e1 + b * e[2] * e[3] + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(4,2) = -b * e[0] * e[1] / 0.2e1 + b * e[2] * e[4] + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[1] * e[3] / 0.2e1 + b * e[0] * e[2] / 0.2e1;
        D(0,3) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[0] * e[3] + b * e[1] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1;
        D(1,3) = b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[2] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[4] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[1] * e[3];
        D(2,3) = b * e[1] * e[4] / 0.2e1 + b * e[2] * e[3] + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(3,3) = a + b * e[2] * e[4] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[3], 0.2e1) + b * pow(e[0], 0.2e1) / 0.2e1;
        D(4,3) = b * e[3] * e[4] + b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(0,4) = -b * e[0] * e[3] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 - b * e[1] * e[2] / 0.2e1 + b * e[0] * e[4] + b * e[1] * e[3] / 0.2e1;
        D(1,4) = b * e[0] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[4] - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1;
        D(2,4) = -b * e[0] * e[1] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[2] * e[4] + b * e[1] * e[3] / 0.2e1 + b * e[0] * e[2] / 0.2e1;
        D(3,4) = b * e[3] * e[4] + b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(4,4) = a + 0.3e1 / 0.4e1 * b * pow(e[4], 0.2e1) + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1;
        break;
    case 6:
        D(0,0) = -b * e[1] * e[2] / 0.2e1 - b * e[3] * e[4] / 0.2e1 - b * e[4] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + a + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1) + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 - b * e[2] * e[3] / 0.2e1;
        D(1,0) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * e[3] * e[5] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[2] * e[4] / 0.2e1 - b * e[1] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1;
        D(2,0) = -b * e[0] * e[3] / 0.2e1 + b * e[0] * e[2] + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 + b * e[2] * e[4] / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * e[1] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1;
        D(3,0) = -b * e[0] * e[4] / 0.2e1 + b * e[0] * e[3] + b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[1] * e[5] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(4,0) = b * e[0] * e[4] - b * e[0] * e[3] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 + b * e[1] * e[5] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(5,0) = -b * e[0] * e[4] / 0.2e1 - b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[5] - b * e[1] * e[3] / 0.2e1 + b * e[1] * e[4] / 0.2e1 + b * e[2] * e[3] / 0.2e1;
        D(0,1) = b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * e[3] * e[5] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[0] * e[1] - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[2] * e[4] / 0.2e1 - b * e[1] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1;
        D(1,1) = -b * e[0] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + a + b * e[0] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1);
        D(2,1) = b * e[0] * e[3] / 0.2e1 + b * e[1] * e[2] + b * e[2] * e[3] / 0.2e1 + b * e[4] * e[5] / 0.2e1 - b * e[1] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[0] * e[4] / 0.2e1;
        D(3,1) = b * e[2] * e[4] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[1] * e[3] + b * e[0] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 - b * e[0] * e[1] / 0.2e1 + b * e[0] * e[4] / 0.2e1;
        D(4,1) = b * e[0] * e[3] / 0.2e1 + b * e[1] * e[4] + b * e[0] * e[5] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1;
        D(5,1) = b * e[2] * e[4] / 0.2e1 - b * e[0] * e[3] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[1] * e[5] + b * e[0] * e[4] / 0.2e1;
        D(0,2) = -b * e[0] * e[3] / 0.2e1 + b * e[0] * e[2] + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 + b * e[2] * e[4] / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * e[1] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1;
        D(1,2) = b * e[0] * e[3] / 0.2e1 + b * e[1] * e[2] + b * e[2] * e[3] / 0.2e1 + b * e[4] * e[5] / 0.2e1 - b * e[1] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 - b * e[0] * e[4] / 0.2e1;
        D(2,2) = 0.3e1 / 0.4e1 * b * pow(e[2], 0.2e1) + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + a - b * e[0] * e[5] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * e[1] * e[3] / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1;
        D(3,2) = -b * pow(e[0], 0.2e1) / 0.4e1 + b * e[3] * e[4] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[0] * e[5] / 0.2e1 + b * e[2] * e[3] + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(4,2) = b * pow(e[3], 0.2e1) / 0.4e1 + b * e[1] * e[5] / 0.2e1 + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[1] * e[3] / 0.2e1 + b * e[2] * e[4] + b * e[0] * e[2] / 0.2e1;
        D(5,2) = b * e[2] * e[5] + b * e[3] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(0,3) = -b * e[0] * e[4] / 0.2e1 + b * e[0] * e[3] + b * e[1] * e[2] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[1] * e[5] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(1,3) = b * e[2] * e[4] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[1] * e[3] + b * e[0] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 - b * e[0] * e[1] / 0.2e1 + b * e[0] * e[4] / 0.2e1;
        D(2,3) = -b * pow(e[0], 0.2e1) / 0.4e1 + b * e[3] * e[4] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[0] * e[5] / 0.2e1 + b * e[2] * e[3] + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(3,3) = b * e[1] * e[5] / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + a + b * e[2] * e[4] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[3], 0.2e1);
        D(4,3) = b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4] + b * e[0] * e[1] / 0.2e1;
        D(5,3) = b * pow(e[4], 0.2e1) / 0.4e1 + b * e[1] * e[3] / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[3] * e[5];
        D(0,4) = b * e[0] * e[4] - b * e[0] * e[3] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 + b * e[1] * e[5] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(1,4) = b * e[0] * e[3] / 0.2e1 + b * e[1] * e[4] + b * e[0] * e[5] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1;
        D(2,4) = b * pow(e[3], 0.2e1) / 0.4e1 + b * e[1] * e[5] / 0.2e1 + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[1] * e[3] / 0.2e1 + b * e[2] * e[4] + b * e[0] * e[2] / 0.2e1;
        D(3,4) = b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4] + b * e[0] * e[1] / 0.2e1;
        D(4,4) = b * e[3] * e[5] / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[4], 0.2e1) + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + a;
        D(5,4) = b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[4] * e[5] + b * e[3] * e[4] / 0.2e1;
        D(0,5) = -b * e[0] * e[4] / 0.2e1 - b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[5] - b * e[1] * e[3] / 0.2e1 + b * e[1] * e[4] / 0.2e1 + b * e[2] * e[3] / 0.2e1;
        D(1,5) = b * e[2] * e[4] / 0.2e1 - b * e[0] * e[3] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[1] * e[5] + b * e[0] * e[4] / 0.2e1;
        D(2,5) = b * e[2] * e[5] + b * e[3] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[1] * e[4] / 0.2e1;
        D(3,5) = b * pow(e[4], 0.2e1) / 0.4e1 + b * e[1] * e[3] / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[3] * e[5];
        D(4,5) = b * e[0] * e[1] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[4] * e[5] + b * e[3] * e[4] / 0.2e1;
        D(5,5) = 0.3e1 / 0.4e1 * b * pow(e[5], 0.2e1) + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + a + b * pow(e[0], 0.2e1) / 0.2e1;
        break;
    case 7:
        D(0,0) = a + b * pow(e[6], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 - b * e[0] * e[1] / 0.2e1 - b * e[4] * e[5] / 0.2e1 - b * e[5] * e[6] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 - b * e[3] * e[4] / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[0], 0.2e1) - b * e[2] * e[3] / 0.2e1;
        D(1,0) = -b * e[1] * e[3] / 0.2e1 + b * e[0] * e[1] + b * e[4] * e[5] / 0.2e1 + b * e[5] * e[6] / 0.2e1 - b * e[2] * e[4] / 0.2e1 - b * e[3] * e[5] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 - b * e[4] * e[6] / 0.2e1 - b * e[0] * e[2] / 0.2e1;
        D(2,0) = b * e[1] * e[3] / 0.2e1 - b * e[3] * e[6] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 - b * e[1] * e[4] / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * e[4] * e[6] / 0.2e1 + b * e[0] * e[2];
        D(3,0) = b * e[3] * e[6] / 0.2e1 - b * e[2] * e[6] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[0] * e[3] - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[1] * e[5] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[1] * e[4] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * e[0] * e[2] / 0.2e1;
        D(4,0) = b * e[1] * e[3] / 0.2e1 + b * e[2] * e[6] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 - b * e[1] * e[2] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[1] * e[5] / 0.2e1 - b * e[1] * e[6] / 0.2e1 + b * e[0] * e[4] - b * e[0] * e[5] / 0.2e1;
        D(5,0) = -b * e[1] * e[3] / 0.2e1 - b * e[0] * e[6] / 0.2e1 - b * pow(e[2], 0.2e1) / 0.4e1 + b * e[1] * e[6] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[4] / 0.2e1 + b * e[0] * e[5];
        D(6,0) = b * e[0] * e[6] + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[2] * e[4] / 0.2e1 + b * e[1] * e[5] / 0.2e1 - b * e[2] * e[3] / 0.2e1 - b * e[1] * e[4] / 0.2e1 - b * e[0] * e[5] / 0.2e1;
        D(0,1) = -b * e[1] * e[3] / 0.2e1 + b * e[0] * e[1] + b * e[4] * e[5] / 0.2e1 + b * e[5] * e[6] / 0.2e1 - b * e[2] * e[4] / 0.2e1 - b * e[3] * e[5] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[3] * e[4] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 - b * e[4] * e[6] / 0.2e1 - b * e[0] * e[2] / 0.2e1;
        D(1,1) = b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 - b * e[3] * e[6] / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * pow(e[6], 0.2e1) / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[1] * e[4] / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[1], 0.2e1) + a + b * pow(e[4], 0.2e1) / 0.2e1;
        D(2,1) = -b * pow(e[0], 0.2e1) / 0.4e1 + b * e[5] * e[6] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[4] * e[5] / 0.2e1 - b * e[1] * e[5] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[2] - b * e[2] * e[6] / 0.2e1;
        D(3,1) = b * e[3] * e[5] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 + b * e[2] * e[4] / 0.2e1 + b * e[4] * e[6] / 0.2e1 - b * e[0] * e[5] / 0.2e1 - b * e[1] * e[6] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * e[1] * e[3] - b * e[0] * e[1] / 0.2e1 + b * e[0] * e[2] / 0.2e1;
        D(4,1) = b * e[0] * e[5] / 0.2e1 + b * e[3] * e[6] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[0] * e[3] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * e[0] * e[6] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[4] - b * pow(e[1], 0.2e1) / 0.4e1;
        D(5,1) = b * pow(e[3], 0.2e1) / 0.4e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * e[1] * e[5] - b * e[0] * e[3] / 0.2e1 + b * e[0] * e[6] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * e[2] * e[6] / 0.2e1;
        D(6,1) = -b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[5] / 0.2e1 + b * e[1] * e[6] - b * e[0] * e[4] / 0.2e1 - b * e[1] * e[3] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1;
        D(0,2) = b * e[1] * e[3] / 0.2e1 - b * e[3] * e[6] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[3] * e[5] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.4e1 - b * e[1] * e[4] / 0.2e1 - b * e[2] * e[5] / 0.2e1 + b * e[4] * e[6] / 0.2e1 + b * e[0] * e[2];
        D(1,2) = -b * pow(e[0], 0.2e1) / 0.4e1 + b * e[5] * e[6] / 0.2e1 - b * e[0] * e[4] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[4] * e[5] / 0.2e1 - b * e[1] * e[5] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[2] - b * e[2] * e[6] / 0.2e1;
        D(2,2) = 0.3e1 / 0.4e1 * b * pow(e[2], 0.2e1) + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[6], 0.2e1) / 0.2e1 - b * e[1] * e[6] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 - b * e[0] * e[5] / 0.2e1 + a + b * pow(e[0], 0.2e1) / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(3,2) = b * e[1] * e[4] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[0] * e[1] / 0.2e1 - b * e[0] * e[6] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[2] * e[3] + b * e[5] * e[6] / 0.2e1 + b * e[0] * e[5] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1;
        D(4,2) = b * pow(e[3], 0.2e1) / 0.4e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[0] * e[6] / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[1] * e[5] / 0.2e1 + b * e[4] * e[6] / 0.2e1 + b * e[2] * e[4] + b * e[1] * e[3] / 0.2e1;
        D(5,2) = b * e[1] * e[4] / 0.2e1 - b * e[0] * e[2] / 0.2e1 + b * e[1] * e[6] / 0.2e1 + b * e[2] * e[5] + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[3] * e[6] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1;
        D(6,2) = b * e[0] * e[4] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[1] * e[5] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.4e1 + b * e[2] * e[6];
        D(0,3) = -b * e[1] * e[5] / 0.2e1 - b * e[2] * e[6] / 0.2e1 + b * e[3] * e[6] / 0.2e1 + b * e[2] * e[5] / 0.2e1 + b * e[1] * e[4] / 0.2e1 - b * e[0] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[0] * e[3];
        D(1,3) = -b * e[0] * e[5] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[3] * e[5] / 0.2e1 - b * e[1] * e[6] / 0.2e1 + b * e[4] * e[6] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * pow(e[2], 0.2e1) / 0.4e1 + b * e[0] * e[2] / 0.2e1 + b * e[1] * e[3];
        D(2,3) = b * e[0] * e[5] / 0.2e1 + b * e[5] * e[6] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[4] / 0.2e1 - b * e[0] * e[6] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] + b * e[1] * e[2] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1;
        D(3,3) = b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[6], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[3], 0.2e1) + b * e[1] * e[5] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[6] / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + a;
        D(4,3) = b * e[5] * e[6] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[6] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4];
        D(5,3) = b * pow(e[4], 0.2e1) / 0.4e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[6] / 0.2e1 + b * e[3] * e[5] + b * e[4] * e[6] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(6,3) = b * e[3] * e[6] + b * e[2] * e[5] / 0.2e1 + b * e[1] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[4] * e[5] / 0.2e1;
        D(0,4) = b * pow(e[2], 0.2e1) / 0.4e1 - b * e[1] * e[2] / 0.2e1 - b * e[1] * e[6] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * e[2] * e[6] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[1] * e[5] / 0.2e1 + b * e[0] * e[4] + b * e[1] * e[3] / 0.2e1;
        D(1,4) = b * e[1] * e[4] + b * e[0] * e[5] / 0.2e1 - b * e[0] * e[2] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[3] / 0.2e1 - b * e[0] * e[6] / 0.2e1 + b * e[3] * e[6] / 0.2e1 + b * e[2] * e[5] / 0.2e1;
        D(2,4) = b * pow(e[3], 0.2e1) / 0.4e1 + b * e[3] * e[5] / 0.2e1 + b * e[2] * e[4] + b * e[0] * e[2] / 0.2e1 + b * e[0] * e[6] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[1] * e[5] / 0.2e1 + b * e[4] * e[6] / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(3,4) = b * e[5] * e[6] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[1] * e[6] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[2] * e[3] / 0.2e1 + b * e[1] * e[2] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4];
        D(4,4) = b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[6], 0.2e1) / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[2] * e[6] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[4], 0.2e1) + a;
        D(5,4) = b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[3] * e[4] / 0.2e1 + b * e[5] * e[6] / 0.2e1 + b * e[4] * e[5] + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[3] * e[6] / 0.2e1;
        D(6,4) = b * pow(e[5], 0.2e1) / 0.4e1 + b * e[3] * e[5] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[4] * e[6] + b * e[1] * e[3] / 0.2e1;
        D(0,5) = -b * pow(e[2], 0.2e1) / 0.4e1 + b * e[1] * e[4] / 0.2e1 + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[5] + b * e[1] * e[6] / 0.2e1 - b * e[0] * e[4] / 0.2e1 - b * e[0] * e[6] / 0.2e1 - b * e[1] * e[3] / 0.2e1;
        D(1,5) = b * e[2] * e[6] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * e[0] * e[4] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.4e1 - b * e[1] * e[2] / 0.2e1 + b * e[0] * e[6] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[1] * e[5];
        D(2,5) = -b * e[0] * e[2] / 0.2e1 + b * e[3] * e[6] / 0.2e1 + b * e[1] * e[4] / 0.2e1 + b * e[0] * e[3] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 + b * e[1] * e[6] / 0.2e1 + b * e[2] * e[5] + b * e[3] * e[4] / 0.2e1;
        D(3,5) = b * pow(e[4], 0.2e1) / 0.4e1 - b * e[0] * e[1] / 0.2e1 + b * e[2] * e[6] / 0.2e1 + b * e[3] * e[5] + b * e[4] * e[6] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] / 0.2e1 + b * e[1] * e[3] / 0.2e1;
        D(4,5) = b * e[1] * e[2] / 0.2e1 - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[3] * e[4] / 0.2e1 + b * e[5] * e[6] / 0.2e1 + b * e[4] * e[5] + b * e[2] * e[3] / 0.2e1 + b * e[0] * e[1] / 0.2e1 + b * e[3] * e[6] / 0.2e1;
        D(5,5) = b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[6], 0.2e1) / 0.2e1 + 0.3e1 / 0.4e1 * b * pow(e[5], 0.2e1) + b * e[4] * e[6] / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[3], 0.2e1) / 0.2e1 + a + b * pow(e[4], 0.2e1) / 0.2e1;
        D(6,5) = b * e[2] * e[3] / 0.2e1 + b * e[5] * e[6] - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1;
        D(0,6) = -b * e[1] * e[4] / 0.2e1 - b * e[0] * e[5] / 0.2e1 + b * pow(e[3], 0.2e1) / 0.4e1 + b * e[0] * e[6] + b * e[1] * e[5] / 0.2e1 + b * e[2] * e[4] / 0.2e1 - b * e[2] * e[3] / 0.2e1;
        D(1,6) = b * e[1] * e[6] + b * e[0] * e[5] / 0.2e1 + b * e[2] * e[5] / 0.2e1 - b * e[0] * e[4] / 0.2e1 - b * pow(e[2], 0.2e1) / 0.4e1 - b * e[1] * e[3] / 0.2e1 + b * e[3] * e[4] / 0.2e1;
        D(2,6) = b * e[0] * e[4] / 0.2e1 - b * e[1] * e[2] / 0.2e1 + b * e[3] * e[5] / 0.2e1 + b * e[1] * e[5] / 0.2e1 - b * e[0] * e[3] / 0.2e1 + b * pow(e[4], 0.2e1) / 0.4e1 + b * e[2] * e[6];
        D(3,6) = b * e[3] * e[6] + b * e[2] * e[5] / 0.2e1 + b * e[1] * e[4] / 0.2e1 - b * pow(e[1], 0.2e1) / 0.4e1 - b * e[0] * e[2] / 0.2e1 + b * e[0] * e[3] / 0.2e1 + b * e[4] * e[5] / 0.2e1;
        D(4,6) = b * pow(e[5], 0.2e1) / 0.4e1 + b * e[3] * e[5] / 0.2e1 + b * e[2] * e[4] / 0.2e1 + b * e[0] * e[2] / 0.2e1 - b * e[0] * e[1] / 0.2e1 + b * e[4] * e[6] + b * e[1] * e[3] / 0.2e1;
        D(5,6) = b * e[2] * e[3] / 0.2e1 + b * e[5] * e[6] - b * pow(e[0], 0.2e1) / 0.4e1 + b * e[1] * e[2] / 0.2e1 + b * e[4] * e[5] / 0.2e1 + b * e[3] * e[4] / 0.2e1 + b * e[0] * e[1] / 0.2e1;
        D(6,6) = b * pow(e[3], 0.2e1) / 0.2e1 + b * pow(e[1], 0.2e1) / 0.2e1 + b * pow(e[2], 0.2e1) / 0.2e1 + b * pow(e[5], 0.2e1) / 0.2e1 + b * pow(e[0], 0.2e1) / 0.2e1 + b * pow(e[4], 0.2e1) / 0.2e1 + a + 0.3e1 / 0.4e1 * b * pow(e[6], 0.2e1);
        break;
    default:
        throw std::string("Coupling order not implemented yet");
    }
}

Coupl::~Coupl()
{
    D.clear();
    N.clear();
}
