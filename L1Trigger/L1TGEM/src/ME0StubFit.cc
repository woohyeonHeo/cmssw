#include "L1Trigger/L1TGEM/interface/ME0StubFit.h"

using namespace l1t::me0;

double l1t::me0::reciprocal6(int n) {
  if (n >= 1 && n <= 6) {
    return RECIP6[n - 1];
  } else {
    return 0.0; // or throw an exception
  }
}
double l1t::me0::reciprocal(int n) {
  if (n >= 1 && n <= 2047) {
    return RECIP[n - 1];
  } else {
    return 0.0; // or throw an exception
  }
}
std::vector<double> l1t::me0::llseFit(const std::vector<double>& x, const std::vector<double>& y) {
  double xSum = 0;
  double ySum = 0;
  for (double val : x) {
    xSum += val;
  }
  for (double val : y) {
    ySum += val;
  }
  int n = x.size();
  // linear regression
  double product = 0;
  double squares = 0;
  for (int i = 0; i < n; ++i) {
    product += (n * x[i] - xSum) * (n * y[i] - ySum);
    squares += (n * x[i] - xSum) * (n * x[i] - xSum);
  }

  double m = product / squares;
  double b = (ySum - m * xSum) / n;
  double sse = 0.0;
  for (int i = 0; i < n; ++i) {
    sse += (y[i] - m * x[i] - b) * (y[i] - m * x[i] - b);
  }

  std::vector<double> fit = {m, b, sse / n};
  return fit;
}
std::vector<double> l1t::me0::vhdlExactFit(const std::vector<int>& centroids, const std::vector<bool>& validMask) {
  // if true not in validMask return 0, 0, 0
  if (std::find(validMask.begin(), validMask.end(), true) == validMask.end()) {
    return {0.0, 0.0, 0.0};
  }

  // remove invalid centroids
  std::vector<int> x;
  std::vector<int> y;
  for (size_t i = 0; i < centroids.size(); ++i) {
    if (validMask[i]) {
      x.push_back(i);
      y.push_back(centroids[i]);
    }
  }

	// Stage 1
  int validCount = x.size();
  ap_uint<4> sumX = std::accumulate(x.begin(), x.end(), 0);
  ap_int<10> sumY = std::accumulate(y.begin(), y.end(), 0);

  std::vector<ap_uint<5>> nX;
  std::vector<ap_int<10>> nY;
  nX.reserve(6);
  nY.reserve(6);
  for (size_t i = 0; i < x.size(); ++i) {
    nX.push_back(validCount * x[i]);
    nY.push_back(validCount * y[i]);
  }

	// Stage 2
  std::vector<ap_int<6>> xDiff;
  std::vector<ap_int<10>> yDiff;
  xDiff.reserve(6);
  yDiff.reserve(6);
  for (size_t i = 0; i < x.size(); ++i) {
    xDiff.push_back(nX[i] - sumX);
    yDiff.push_back(nY[i] - sumY);
  }

  // Stage 3
  std::vector<ap_int<15>> product;
  std::vector<ap_int<12>> square;
  for (size_t i = 0; i < xDiff.size(); ++i) {
    product.push_back(xDiff[i] * yDiff[i]);
    square.push_back(xDiff[i] * xDiff[i]);
  }

	// Stage 4-5
  ap_int<13> sumProduct = std::accumulate(product.begin(), product.end(), 0);
  ap_int<13> sumSquare = std::accumulate(square.begin(), square.end(), 0);

  ap_fixed<14, 14> sumProductFixed = sumProduct;
  ap_fixed<15, 2> sumSquareReciprocal = reciprocal(static_cast<int>(sumSquare));

	// Stage 6-7
	ap_fixed<29, 16> slopeTemp = sumProductFixed * sumSquareReciprocal; 
	ap_fixed<10, 4> slope = slopeTemp;
	
	ap_fixed<6, 6> sumXFixed = sumX;

	// Stage 8
	ap_fixed<16, 10> slopeMult = slope * sumXFixed;

	// Stage 9
	ap_fixed<15, 8> slopeTimesX = slopeMult;

	// Stage 10, 11, 12
	ap_fixed<32, 11> interceptMult 
    = static_cast<ap_fixed<16, 2>>(reciprocal6(validCount)) * ( static_cast<ap_fixed<8, 8>>(sumY) - slopeTimesX );
	ap_fixed<20, 8> slopeS10Mult = slope * 5.0;

	ap_fixed<15, 7> slopeS11X5 = slopeS10Mult;

	ap_fixed<15, 7> slopeS12X2p5 = slopeS11X5/2.0;

	ap_fixed<15, 7> intercept = interceptMult;

	// Stage 13 : Output
	ap_fixed<10, 5> stripOut = slopeS12X2p5 + intercept;
	ap_fixed<14, 7> interceptOut = intercept;
	ap_fixed<10, 4> slopeOut = slope;

	return {slopeOut.to_double(), interceptOut.to_double(), stripOut.to_double()};
}