// libstdaudio
// Copyright (c) 2018 - Timur Doumler
// Copyright (c) 2019 - Frank Birbacher
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <cmath>
#include <array>
#include <thread>
#include <audio>
#include <coaudio>

// This example app plays a short melody using a simple square wave synthesiser.

constexpr std::array<int, 22> notes = {
  88, 86, 78, 78, 80, 80,
  85, 83, 74, 74, 76, 76,
  83, 81, 73, 73, 76, 76,
  81, 81, 81, 81
};

float note_to_frequency_hz(int note) {
  constexpr float pitch_standard_hz = 440.0f;
  return pitch_standard_hz * std::pow(2.0f, float(note - 69) / 12.0f);
}

using std::experimental::audio::generator;

constexpr float m2pif = 2. * M_PI;

generator<float> synth(const int bpm, const int sample_rate)
{
  const int note_duration_samples = sample_rate * 60.f / bpm;
  const float frequency_factor = m2pif / sample_rate;

  float phase = 0;
  for (auto const note : notes) {

    const float delta = frequency_factor * note_to_frequency_hz(note);
    for (int i = 0; i < note_duration_samples; ++i) {

      co_yield std::copysign(0.1f, std::sin(phase));
      phase = std::fmod(phase + delta, m2pif);

    }
  }
}


int main() {
  using namespace std::experimental::audio;

  auto d = get_default_output_device();
  const auto rate = d.get_sample_rate();

  auto g = synth(260, rate);

  std::atomic<bool> stop{false};
  installGenerator(d, g, stop);

  d.start();
  while (!stop.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  d.stop();
}