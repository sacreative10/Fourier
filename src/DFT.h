//
// Created by sohaibalam on 19/11/23.
//

#ifndef CMAKESFMLPROJECT_DFT_H
#define CMAKESFMLPROJECT_DFT_H

#include <complex>
#include <vector>

struct DFTResponse {
  std::vector<std::complex<double>> complexValues;
  std::vector<double> frequencies;
  std::vector<double> amplitudes;
  std::vector<double> phases;
};

DFTResponse DFT(const std::vector<std::vector<double>> &input) {
  const auto N = input.size();
  DFTResponse response;
  for (int k = 0; k < N; k++) {
    std::complex<double> sum = 0;
    for (int n = 0; n < N; n++) {
      const double phi = (2 * M_PI * k * n) / N;
      std::complex<double> c(cos(phi), -sin(phi));
      sum += std::complex<double>(input[n][0], input[n][1]) * c;
    }
    sum /= N;
    response.complexValues.push_back(sum);
    response.frequencies.push_back(k);
    response.amplitudes.push_back(std::abs(sum));
    response.phases.push_back(std::arg(sum));
  }
  return response;
}

#endif // CMAKESFMLPROJECT_DFT_H
