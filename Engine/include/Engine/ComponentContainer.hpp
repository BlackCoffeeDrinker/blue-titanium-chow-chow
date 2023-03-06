#pragma once

#include <map>
#include <type_traits>

#include "Detail/TypeId.hpp"
#include "Component.hpp"

namespace e00 {
/**
 * Components can be used to extend a class without touching the class source code,
 *
 */
class ComponentContainer {
  std::map<type_t, Component *> _components;

  template<typename ComponentType>
  constexpr void Validate() const {
    static_assert(std::is_convertible<ComponentType *, Component *>::value, "Class must be subclass of Component");
  }

  [[nodiscard]] Component *GetComponent(const type_t &id) const {
    if (const auto it = _components.find(id);
        it != _components.end()) {
      return it->second;
    }

    return nullptr;
  }

public:
  ComponentContainer() = default;

  ~ComponentContainer() {
    for (auto [type, component] : _components) {
      delete component;
      component = nullptr;
    }

    _components.clear();
  }

  [[nodiscard]] bool HasComponentOfType(type_t id) const noexcept {
    return _components.count(id) > 0;
  }

  template<typename T>
  [[nodiscard]] bool HasComponent() const noexcept {
    Validate<T>();
    return HasComponentOfType(type_id<T>());
  }

  /**
   * Gets a component by type
   *
   * @tparam T Component type
   * @return the component or null
   */
  template<class T>
  [[nodiscard]] T *GetComponent() const noexcept {
    Validate<T>();

    const auto &it = _components.find(type_id<T>());
    if (it != _components.end()) {
      return static_cast<T *>(it->second);
    }

    return nullptr;
  }

  /**
   *
   * @tparam T
   * @tparam Args
   * @param args
   * @return
   */
  template<typename T, typename... Args>
  T *CreateComponent(Args &&...args) {
    Validate<T>();

    if (HasComponent<T>()) {
      return nullptr;
    }

    const auto it = _components.emplace(
      std::make_pair(type_id<T>(),
        new T(std::forward<Args>(args)...)));

    return static_cast<T *>(it.first->second);
  }

  /**
   *
   * @tparam T
   */
  template<typename T>
  void RemoveComponent() {
    Validate<T>();
    _components.erase(type_id<T>());
  }
};
}// namespace e00
