#pragma once

#include "Detail/TypeId.hpp"

namespace e00 {
/**
 * Base to components that can be inserted into any ComponentContainer
 *
 */
class Component {
  type_t _type;

public:
  explicit Component(type_t type) : _type(type) {}

  virtual ~Component() = default;

  [[nodiscard]] type_t Type() const noexcept { return _type; }
};

template<typename T>
class ComponentT : public Component {
public:
  ComponentT() : Component(type_id<T>()) {}
  ~ComponentT() override = default;

  /**
   * Helper for casting components
   * @param component
   * @return
   */
  static constexpr T *CastTo(Component *component) noexcept {
    if (component && component->Type() == type_id<T>()) {
      return static_cast<T *>(component);
    }
    return nullptr;
  }
};
}// namespace e00
