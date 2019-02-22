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

constexpr float bpm = 260.0;

float note_to_frequency_hz(int note) {
  constexpr float pitch_standard_hz = 440.0f;
  return pitch_standard_hz * std::pow(2.0f, float(note - 69) / 12.0f);
}

using std::experimental::audio::generator;

generator<float> synth(const float _sample_rate)
{
  const float _note_duration_ms = 60'000.0f / bpm;
  float _phase = 0;
  for (auto const note : notes) {
    float frequency_hz = note_to_frequency_hz(note);
    float _delta = 2.0f * frequency_hz * static_cast<float>(M_PI / _sample_rate);
    for (float _ms_counter = 0;
        _ms_counter < _note_duration_ms;
        _ms_counter += 1000. / _sample_rate) {
      auto next_sample = std::copysign(0.1f, std::sin(_phase));
      _phase = std::fmod(_phase + _delta, 2.0f * float(M_PI));
      co_yield next_sample;
    }
  }
}


int main() {
  using namespace std::experimental::audio;

  auto d = get_default_output_device();
  const auto rate = d.get_sample_rate();

  auto g = synth(rate);

  std::atomic<bool> stop{false};
  playGenerated(d, g, stop);

  d.start();
  while (!stop.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  d.stop();
}