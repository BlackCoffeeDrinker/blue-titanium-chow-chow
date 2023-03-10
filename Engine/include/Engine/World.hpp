#pragma once

#include <list>
#include <system_error>
#include <chrono>

#include "Actor.hpp"
#include "Engine/Resource/Map.hpp"
#include "Engine/Resource/Tileset.hpp"

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

  ResourcePtrT<Map> _map;
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

      return _world->_map->Tileset().template GetComponent<T>(MapToReal(position));
    }
  };

  explicit World(std::string name);

  ~World();

  void SetMap(ResourcePtrT<Map>&& map) { _map = std::move(map); }

  [[nodiscard]] auto Size() const { return _map->Size(); }
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

  /**
   * Processes a delta tick
   *
   * @param delta time since last tick
   */
  void Tick(const std::chrono::milliseconds &delta) noexcept;
};
}// namespace e00
