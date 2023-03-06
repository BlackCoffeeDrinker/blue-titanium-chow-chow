#pragma once

namespace e00 {
/**
 * An actor is an item in a world; it's anything that can interact
 * or be interacted with. It lives in a world and acts as a rigid
 * body
 */
class Actor : public ComponentContainer {
public:
  enum class BodyType {
    Static,// Unmovable actor
    Dynamic,// Actor can move
  };

private:
  Vec2D<WorldCoordinateType> _size;// << Size of this actor
  BodyType _type;

public:
  void Size(const Vec2D<WorldCoordinateType> &newSize) { _size = newSize; }

  /**
   * Size of this actor
   * @return
   */
  [[nodiscard]] auto Size() const { return _size; }

  /**
   *
   * @return
   */
  [[nodiscard]] auto Type() const noexcept { return _type; }
};
}// namespace e00
