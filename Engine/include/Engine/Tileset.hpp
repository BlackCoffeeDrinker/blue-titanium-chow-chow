#pragma once

namespace e00 {
class Tileset {
  struct Tile {
    ComponentContainer component_container;
  };

  std::vector<Tile> _tiles;
  ResourcePtrT<e00::Bitmap> _tileset;

public:
  typedef decltype(_tiles)::size_type TileIdType;

  Tileset() = default;

  explicit Tileset(TileIdType nbTiles);

  Tileset(const Tileset&);

  Tileset(Tileset&&) noexcept;

  ~Tileset();

  Tileset& operator=(const Tileset&);

  Tileset& operator=(Tileset&&) noexcept;

  explicit operator bool() const noexcept { return !_tiles.empty(); }

  template<typename T>
  auto GetComponent(TileIdType tileId) const -> decltype(_tiles.at(tileId).component_container.GetComponent<T>()) {
    return _tiles.at(tileId).component_container.template GetComponent<T>();
  }

  template<typename T, typename ...Args>
  T* CreateComponent(TileIdType tileId, Args&&...args) {
    return _tiles.at(tileId).component_container.template CreateComponent<T, Args...>(std::forward<Args>(args)...);
  }

  template<typename T>
  [[nodiscard]] bool HasComponent(TileIdType tileId) const {
    return _tiles.at(tileId).component_container.template HasComponent<T>();
  }

  template<typename T>
  void RemoveComponent(TileIdType tileId) {
    _tiles.at(tileId).component_container.template RemoveComponent<T>();
  }
};
}
