#include "attack.h"

#include "barrage.h"

#include <iostream>
#include <stdexcept>
#include <uhd/stream.hpp>

namespace {

void transmission_continuous(uhd::usrp::multi_usrp::sptr usrp,
                           const all_args_t &args) {
  uhd::stream_args_t stream_args("fc32", "sc16");
  uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

  uhd::tx_metadata_t metadata;
  metadata.start_of_burst = true;
  metadata.end_of_burst = false;
  metadata.has_time_spec = false;

  const std::vector<std::complex<float>> samples = generate_samples(args);

  while (true) {
    tx_stream->send(samples.data(), samples.size(), metadata);
    std::cout << "Transmitting (" << attackTypeToString(args.attack_type)
              << ")...." << std::endl;
    metadata.start_of_burst = false;
  }
}

} // namespace

std::vector<std::complex<float>> generate_samples(const all_args_t &args) {
  switch (args.attack_type) {
  case attack_type_t::BARRAGE:
    return generate_barrage(args);
  case attack_type_t::CONSTANT:
  case attack_type_t::RANDOM:
    throw std::runtime_error("Attack type not implemented yet: " +
                             std::string(attackTypeToString(args.attack_type)));
  }

  return generate_barrage(args);
}

void transmission(uhd::usrp::multi_usrp::sptr usrp, const all_args_t &args) {
  switch (args.attack_type) {
  case attack_type_t::BARRAGE:
    transmission_continuous(usrp, args);
    return;
  case attack_type_t::CONSTANT:
  case attack_type_t::RANDOM:
    throw std::runtime_error("Attack type not implemented yet: " +
                             std::string(attackTypeToString(args.attack_type)));
  }
}
