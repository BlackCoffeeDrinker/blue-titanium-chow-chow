#pragma once

#include <map>
#include <type_traits>
#include <functional>

#include "Detail/TypeId.hpp"

namespace e00 {
/**
 * Components can be used to extend a class without touching the class source code,
 *
 */
class ComponentRegistry {
  struct Component {
    virtual void* Get() = 0;
    virtual ~Component() = default;
  };

  std::map<type_t, Component*> _list;

public:

  ~ComponentRegistry() {
    for (auto& it : _list) {
      delete it.second;
      it.second = nullptr;
    }
  }

  template<typename T>
  [[nodiscard]] bool HasComponent() const {
    return _list.find(type_id<T>()) != _list.end();
  }

  template<typename T>
  void RemoveComponent() {
    if (auto& it = _list.find(type_id<T>()); it != _list.end()) {
      delete it->second;
      _list.erase(it);
    }
  }

  template<typename T, typename... Args>
  T *CreateComponent(Args&& ...args) {
    struct TComponent : Component {
      std::unique_ptr<T> t;

      TComponent(std::unique_ptr<T> &&t) : t(std::move(t)) {}
      void* Get() override { return t.get(); }
      ~TComponent() override {}
    };

    if (HasComponent<T>()) return nullptr;

    auto tuptr = std::make_unique<T>(std::forward<Args>(args)...);
    auto ret = _list.emplace(type_id<T>(), new TComponent(std::move(tuptr)));

    return static_cast<T*>(ret.first->second->Get());
  }

  template<typename T>
  T* GetComponent() const {
    if (auto& it = _list.find(type_id<T>()); it != _list.end()) {
      return static_cast<T*>(it->second->Get());
    }

    return nullptr;
  }
};
}// namespace e00
