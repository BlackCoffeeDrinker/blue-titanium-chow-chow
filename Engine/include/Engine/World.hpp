#pragma once

#include <list>
#include <system_error>
#include <chrono>

#include "Actor.hpp"
#include "Map.hpp"
#include "Tileset.hpp"

namespace e00 {
/**
 * A `World` is a
 */
class World {
  struct WorldActor {
    Actor* actor;
    Vec2D<WorldCoordinateType> position;
  };

  std::string _name;

  Map _map;
  Tileset _tileset;
  std::list<WorldActor> _actors;

  template<typename T>
  constexpr static T computeStartForCenter(const T &center, const T &wanted, const T &max) {
    const auto half = wanted / 2;
    if ((half + center) > max) {
      return max - wanted;
    } else if (half > center) {
      return 0;
    } else {
      return center - half;
    }
  }

public:
  class Window {
    friend World;

    const World *_world;
    Vec2D<WorldCoordinateType> _start;
    Vec2D<WorldCoordinateType> _size;

    Window(const World *world, const Vec2D<WorldCoordinateType>& start, const Vec2D<WorldCoordinateType>& size)
      : _world(world),
        _start(start),
        _size(size) {}

  public:
    [[nodiscard]] auto Width() const { return _size.x; }
    [[nodiscard]] auto Height() const { return _size.y; }

    [[nodiscard]] auto MapToReal(const Vec2D<WorldCoordinateType>& position) const {
      if (position > _size)
        return _size;

      return position + _start;
    }

    template<typename T>
    auto GetComponent(const Vec2D<WorldCoordinateType>& position) const {
      if (position > _size)
        return (T *)(nullptr);

      return _world->template GetComponent<T>(MapToReal(position));
    }
  };

  explicit World(std::string name);

  World(Tileset &&t, Map &&m);

  ~World();

  void SetMap(Map&& map) { _map = std::move(map); }

  void SetTileset(Tileset &&tileset) { _tileset = std::move(tileset); }

  explicit operator bool() const noexcept { return _map.operator bool() && _tileset.operator bool(); }

  [[nodiscard]] auto Size() const { return _map.Size(); }

  [[nodiscard]] auto Width() const { return Size().x; }

  [[nodiscard]] auto Height() const { return Size().y; }

  [[nodiscard]] Window MakeWindow(const Vec2D<WorldCoordinateType>& center, const Vec2D<WorldCoordinateType>& size) const {
    // Adjust the "viewport"; don't go over the map
    Vec2D<WorldCoordinateType> adjSize(size.Clamp(Size()));

    // Compute window
    Vec2D<WorldCoordinateType> start = {
      computeStartForCenter(center.x, adjSize.x, Width()),
      computeStartForCenter(center.y, adjSize.y, Height())
    };

    // Build world window
    return { this, start, adjSize };
  }

  /**
   * Inserts actor `actor` at position `position`
   *
   * @param actor the Actor to add to this world
   * @param position the initial position the actor is at
   * @return
   */
  std::error_code Insert(Actor* actor, const Vec2D<WorldCoordinateType>& position);

  template<typename T>
  auto GetComponent(const Vec2D<WorldCoordinateType>& position) const {
    return _tileset.template GetComponent<T>(_map.Get(position));
  }

  template<typename T, typename... Args>
  T *CreateComponentAt(const Vec2D<WorldCoordinateType>& position, Args &&...args) {
    return _tileset.template CreateComponent<T, Args...>(_map.Get(position), std::forward<Args>(args)...);
  }

  template<typename T>
  [[nodiscard]] bool HasComponent(const Vec2D<WorldCoordinateType>& position) const {
    return _tileset.template HasComponent<T>(_map.Get(position));
  }

  template<typename T, typename... Args>
  T *CreateComponent(Tileset::TileIdType tileId, Args &&...args) {
    return _tileset.template CreateComponent<T, Args...>(tileId, std::forward<Args>(args)...);
  }

  template<typename T>
  void RemoveComponent(const Vec2D<WorldCoordinateType>& position) {
    _tileset.template RemoveComponent<T>(_map.Get(position));
  }

  /**
   * Processes a delta tick
   *
   * @param delta time since last tick
   */
  void Tick(const std::chrono::milliseconds &delta) noexcept;
};
}// namespace e00
