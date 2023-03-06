#include <Engine.hpp>

namespace e00 {
Tileset::Tileset(TileIdType nbTiles)
  : _tileset_size({ 0, 0 }),
    _tiles(nbTiles),
    _tileset(nullptr),
    _margin(0),
    _spacing(0) {}

Tileset::~Tileset() = default;

Tileset::Tileset(const Tileset &other)
  : _tileset_size(other._tileset_size),
    _tiles(other._tiles),
    _tileset(other._tileset),
    _margin(other._margin),
    _spacing(other._spacing) {
}

Tileset::Tileset(Tileset &&rhs) noexcept
  : _tileset_size(rhs._tileset_size),
    _tiles(std::move(rhs._tiles)),
    _tileset(std::move(rhs._tileset)),
    _margin(rhs._margin),
    _spacing(rhs._spacing) {
}

Tileset &Tileset::operator=(const Tileset &rhs) {
  if (&rhs != this) {
    _tileset_size = rhs._tileset_size;
    _tiles = rhs._tiles;
    _tileset = rhs._tileset;
    _margin = rhs._margin;
    _spacing = rhs._spacing;
  }
  return *this;
}

Tileset &Tileset::operator=(Tileset &&rhs) noexcept {
  if (&rhs != this) {
    _tileset_size = rhs._tileset_size;
    _tiles = std::move(rhs._tiles);
    _tileset = std::move(rhs._tileset);
    _margin = rhs._margin;
    _spacing = rhs._spacing;
  }
  return *this;
}

}// namespace e00
