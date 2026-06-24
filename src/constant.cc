#include "constant.h"

#include <cmath>

namespace {
constexpr float PI = std::acos(-1.0f);
} // namespace

std::vector<std::complex<float>> generate_constant(const all_args_t &args) {
  std::vector<std::complex<float>> samples;
  samples.reserve(args.num_samples);

  const float delta_t = 1.0f / args.sampling_freq;
  const float tone_hz = args.center_frequency + args.tone_offset_hz;
  float phase = args.initial_phase;

  for (size_t i = 0; i < args.num_samples; ++i) {
    samples.push_back(std::polar(args.amplitude, phase));

    phase += 2.0f * PI * tone_hz * delta_t;
    phase = std::fmod(phase, 2.0f * PI);
    if (phase < 0.0f) {
      phase += 2.0f * PI;
    }
  }

  return samples;
}
