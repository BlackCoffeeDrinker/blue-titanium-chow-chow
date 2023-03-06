#pragma once

#include <Engine.hpp>

namespace e00::impl {
class WorldLoader {
  mutable Logger _logger;

  // Hide the actual implementation, because it adds too much compilation time otherwise
  class Impl;
  Impl* _impl;

  const std::string& _name;
  const std::unique_ptr<Stream>& _input;

public:
  WorldLoader(const std::string& name, const std::unique_ptr<Stream>& worldStream);

  ~WorldLoader();

  bool is_valid() const;

  World build() const;

};
}// namespace e00::impl
