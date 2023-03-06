#pragma once

#include <type_traits>
#include <cstddef>

#include "Resource.hpp"

namespace e00 {
class Engine;

namespace detail {
  struct ControlBlock {
    Resource *resource;
    uint32_t refs;

    virtual ~ControlBlock() = default;

    virtual std::string name() const = 0;
    virtual type_t type() const = 0;
    virtual void zero_refs() = 0;
    virtual std::error_code load() = 0;
  };
}// namespace detail

template<typename T>
struct ResourcePtrT {
  static_assert(std::is_base_of_v<Resource, T>, "Class must be of type resource");
  using pointer = std::remove_reference_t<std::remove_cv_t<T>> *;

  ResourcePtrT() : cb(nullptr) {}
  explicit ResourcePtrT(std::nullptr_t) : cb(nullptr) {}
  ResourcePtrT(const ResourcePtrT &copy) : cb(copy.cb) { AddRef(); }
  ResourcePtrT(ResourcePtrT &&move) noexcept : cb(move.cb) { move.cb = nullptr; }

  ~ResourcePtrT() {
    RemoveRef();
    if (cb && cb->refs == 0) cb->zero_refs();
    cb = nullptr;
  }

  ResourcePtrT &operator=(const ResourcePtrT &other) {
    if (this != &other) {
      RemoveRef();
      cb = other.cb;
      AddRef();
    }
    return *this;
  }

  ResourcePtrT &operator=(ResourcePtrT &&other) noexcept {
    if (this != &other) {
      RemoveRef();
      cb = other.cb;
      other.cb = nullptr;
    }
    return *this;
  }

  bool operator==(const ResourcePtrT &other) const {
    return cb != nullptr && other.cb == cb;
  }

  bool operator!=(const ResourcePtrT &other) const {
    return cb == nullptr || cb != other.cb;
  }

  [[nodiscard]] std::string Name() const {
    return (cb) ? cb->name() : "";
  }

  [[nodiscard]] auto UseCount() const { return (cb) ? cb->refs : 0; }

  pointer Get() const {
    if (!cb) return nullptr;
    if (!cb->resource) cb->load();
    return static_cast<T *>(cb->resource);
  }

  pointer operator*() const { return Get(); }
  pointer operator->() const { return Get(); }

  std::error_code EnsureLoad() {
    if (cb && !cb->resource) return cb->load();
    return {};
  }

private:
  friend class e00::Engine;
  detail::ControlBlock *cb;

  explicit ResourcePtrT(detail::ControlBlock *cb) : cb(cb) { AddRef(); }

  void AddRef() {
    if (cb) cb->refs++;
  }

  void RemoveRef() {
    if (cb && cb->refs > 0) cb->refs--;
  }
};
}// namespace e00
