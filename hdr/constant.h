#ifndef CONSTANT_H
#define CONSTANT_H

#include "args.h"

#include <complex>
#include <vector>

// Constant (spot/CW) jamming: single baseband tone at tone_offset_hz from LO
// (USRP RF is tuned to center_frequency in main.cc).
std::vector<std::complex<float>> generate_constant(const all_args_t &args);

#endif
