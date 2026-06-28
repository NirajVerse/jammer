#ifndef CONSTANT_H
#define CONSTANT_H

#include "args.h"

#include <complex>
#include <vector>

// Constant (partial-band noise) jamming: narrowband noise centered at
// tone_offset_hz with width jam_bandwidth_hz. Synthesized as a sum of
// num_tones randomly-phased tones (defeats DC cancellation, hits many
// subcarriers, sustained degradation without saturating the RX).
std::vector<std::complex<float>> generate_constant(const all_args_t &args);

#endif
