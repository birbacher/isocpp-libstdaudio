// libstdaudio
// Copyright (c) 2019 - Frank Birbacher
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

_LIBSTDAUDIO_NAMESPACE_BEGIN

namespace stdco = std::experimental;

template <typename T>
struct generator {
  struct promise_type {
    T current_value;
    stdco::suspend_always yield_value(T value) {
      this->current_value = value;
      return {};
    }
    stdco::suspend_always initial_suspend() { return {}; }
    stdco::suspend_always final_suspend() { return {}; }
    generator get_return_object() { return generator{this}; };
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
  };

  struct iterator {
    stdco::coroutine_handle<promise_type> handle;
    T currentValue;

    iterator()
    : handle(nullptr)
    {}

    iterator(stdco::coroutine_handle<promise_type> coro)
    : handle(coro)
    {
      handle.resume();
      currentValue = handle.promise().current_value;
    }

    iterator &operator++()
    {
      handle.resume();
      if (handle.done()) handle = nullptr;
      else currentValue = handle.promise().current_value;
      return *this;
    }

    iterator operator++(int)
    {
      auto res = *this;
      ++*this;
      return res;
    }

    bool operator==(iterator const &_Right) const
    {
      return !handle == !_Right.handle;
    }

    bool operator!=(iterator const &_Right) const { return !(*this == _Right); }
    T const &operator*() const { return currentValue; }
    T const *operator->() const { return &currentValue; }
  };

  iterator begin() {
    return handle && !handle.done() ? iterator{handle} : end();
  }

  iterator end() { return {}; }

  generator() noexcept : handle() {}
  generator(generator const&) = delete;
  generator(generator &&rhs) noexcept : handle(rhs.handle) { rhs.handle = nullptr; }
  generator& operator=(generator rhs) noexcept { handle.swap(rhs.handle); return *this; }

  ~generator() {
    if (handle)
      handle.destroy();
  }

private:
  explicit generator(promise_type *p)
  : handle(stdco::coroutine_handle<promise_type>::from_promise(*p))
  {}

  stdco::coroutine_handle<promise_type> handle;
};

void installGenerator(device &dev, generator<float> &g, std::atomic<bool> &done);

_LIBSTDAUDIO_NAMESPACE_END
