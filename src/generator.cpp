// libstdaudio
// Copyright (c) 2019 - Frank Birbacher
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <coaudio>

_LIBSTDAUDIO_NAMESPACE_BEGIN

void installGenerator(device &dev, generator<float> &g, std::atomic<bool> &done)
{
  dev.connect([&g, &done] (device &d, buffer_list& bl) {
    auto it = g.begin();
    for (auto& buffer : bl.output_buffers()) {
      for (auto& frame : buffer.frames()) {

        auto const value = it != g.end() ? *it++ : 0;

        for (auto& sample : frame) {
          sample = value;
        }
      }
    }

    if (it == g.end()) {
      done = true;
    }
  });
}

_LIBSTDAUDIO_NAMESPACE_END
