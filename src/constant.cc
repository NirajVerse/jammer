#include "constant.h"

#include <cmath>
#include <random>
#include <vector>

namespace {
constexpr float PI = std::acos(-1.0f);
} // namespace

// Constant (partial-band noise) jammer.
//
// A pure CW tone is a poor jammer for OFDM:
//   * NR leaves the DC subcarrier empty
//   * USRP RX has automatic DC-offset cancellation
//   * a single tone hits only one of ~3276 subcarriers; LDPC repairs it trivially
//   * the only way a CW can hurt is by RF saturation, which causes RRC release
//
// Instead we synthesize narrowband Gaussian-like noise centered at tone_offset_hz
// with width jam_bandwidth_hz, using a sum of N closely-spaced tones with
// independent random phases. This is "partial-band noise jamming" — hits many
// subcarriers at moderate level, looks like noise to the RX (AGC can't reject),
// and produces sustained, gradable degradation without saturating the front end.
std::vector<std::complex<float>> generate_constant(const all_args_t &args) {
  std::vector<std::complex<float>> samples;
  samples.reserve(args.num_samples);

  const float delta_t = 1.0f / args.sampling_freq;
  const int   num_tones = std::max(1, args.num_tones);
  const float jam_bw    = args.jam_bandwidth_hz;
  const float center_offset = args.tone_offset_hz;

  // Per-tone frequencies uniformly spread across the jam band, plus random
  // initial phases so the sum looks like band-limited noise.
  std::vector<float> tone_freq(num_tones);
  std::vector<float> phase(num_tones);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> phase_dist(0.0f, 2.0f * PI);

  if (num_tones == 1) {
    tone_freq[0] = center_offset;
  } else {
    const float start = center_offset - jam_bw / 2.0f;
    const float step  = jam_bw / static_cast<float>(num_tones - 1);
    for (int k = 0; k < num_tones; ++k) {
      tone_freq[k] = start + step * static_cast<float>(k);
    }
  }
  for (int k = 0; k < num_tones; ++k) {
    phase[k] = phase_dist(gen);
  }

  // Per-tone amplitude is scaled by 1/sqrt(N) so the aggregate peak amplitude
  // stays comparable to args.amplitude and we don't clip the DAC.
  const float per_tone_amp =
      args.amplitude / std::sqrt(static_cast<float>(num_tones));

  for (size_t i = 0; i < args.num_samples; ++i) {
    float re = 0.0f;
    float im = 0.0f;
    for (int k = 0; k < num_tones; ++k) {
      re += per_tone_amp * std::cos(phase[k]);
      im += per_tone_amp * std::sin(phase[k]);
      phase[k] += 2.0f * PI * tone_freq[k] * delta_t;
      if (phase[k] > 2.0f * PI) {
        phase[k] = std::fmod(phase[k], 2.0f * PI);
      } else if (phase[k] < 0.0f) {
        phase[k] = std::fmod(phase[k], 2.0f * PI) + 2.0f * PI;
      }
    }
    samples.emplace_back(re, im);
  }

  return samples;
}
