#include <Engine.hpp>

namespace e00 {
Tileset::Tileset(TileIdType nbTiles) : _tiles(nbTiles) {}

Tileset::~Tileset() = default;

Tileset::Tileset(const Tileset &other) : _tiles(other._tiles) {
}

Tileset::Tileset(Tileset && rhs) noexcept : _tiles(std::move(rhs._tiles)) {
}

Tileset &Tileset::operator=(const Tileset &rhs) {
  if (&rhs != this) {
    _tiles = rhs._tiles;
  }
  return *this;
}

Tileset &Tileset::operator=(Tileset &&rhs) noexcept {
  if (&rhs != this) {
    _tiles = std::move(rhs._tiles);
  }
  return *this;
}
}// namespace e00
