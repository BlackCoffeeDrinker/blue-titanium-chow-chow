#pragma once

#include <Engine.hpp>
#include "Resource/ResourceLoader.hpp"

namespace e00::impl {
class WorldLoader : public ResourceLoader {
  mutable Logger _logger;

  // Hide the actual implementation, because it adds too much compilation time otherwise
  class Impl;
  Impl* _impl;

public:
  WorldLoader();

  ~WorldLoader() override;

  bool CanLoad(const std::unique_ptr<Stream> &stream) override;

  Result ReadLoad(const std::string &name, const std::unique_ptr<Stream> &stream) override;
};
}// namespace e00::impl
