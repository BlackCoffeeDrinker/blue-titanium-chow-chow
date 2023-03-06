#include <Engine.hpp>
#include "Engine/World.hpp"

namespace e00 {
World::World(std::string name)
  : _name(std::move(name)),
    _map() {
}

World::~World() = default;

std::error_code World::Insert(Actor *actor, const Vec2D<WorldCoordinateType> &position) {
  // Is this actor in this world ?
  if (Size() < position) {
    // TODO: Return specific error
    return std::make_error_code(std::errc::invalid_argument);
  }

  _actors.push_back({actor, position});

  return {};
}
void World::Tick(const std::chrono::milliseconds &delta) noexcept {
  for (auto& actor : _actors) {
    
  }
}

}// namespace e00
