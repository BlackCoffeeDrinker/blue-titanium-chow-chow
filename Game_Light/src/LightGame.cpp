
#include <Engine.hpp>
#include <memory>
#include "LightGameEngine.hpp"

std::unique_ptr<e00::Engine> CreateGameEngine() {
  return std::make_unique<light::LightGameEngine>();
}
