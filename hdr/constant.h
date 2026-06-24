#ifndef CONSTANT_H
#define CONSTANT_H

#include "args.h"

#include <complex>
#include <vector>

// Constant (spot/CW) jamming: single tone at center_frequency + tone_offset_hz.
std::vector<std::complex<float>> generate_constant(const all_args_t &args);

#endif
