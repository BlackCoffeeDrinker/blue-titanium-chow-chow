#pragma once

#include "Resource.hpp"
#include <type_traits>

namespace e00 {
class ResourceManager;

namespace detail {
  struct ResourcePtrInternal {
    friend class e00::ResourceManager;

    virtual ~ResourcePtrInternal() noexcept {
      remove_ref();
    }

    [[nodiscard]] std::string Name() const;

  protected:
    struct ControlBlock {
      ResourceManager *owner;
      uint32_t refs;
    };
    ControlBlock *cb;

    constexpr explicit ResourcePtrInternal(nullptr_t) : cb(nullptr) {}

    explicit ResourcePtrInternal(ControlBlock *cb) : cb(cb) { add_ref(); }

    ResourcePtrInternal(const ResourcePtrInternal &copy) : cb(copy.cb) { add_ref(); }

    constexpr ResourcePtrInternal(ResourcePtrInternal &&move) noexcept
      : cb(move.cb) {
      move.cb = nullptr;
    }

    void copy_from(const ResourcePtrInternal &other) {
      remove_ref();
      cb = other.cb;
      add_ref();
    }

    void move_from(ResourcePtrInternal &&other) noexcept {
      remove_ref();
      cb = other.cb;
      other.cb = nullptr;
    }

    Resource *load();

  private:
    void add_ref();

    void remove_ref();
  };
}// namespace detail


template<typename T>
struct ResourcePtrT : public detail::ResourcePtrInternal {
  friend class ResourceManager;

  using pointer = std::remove_reference_t<T> *;
  using element_type = T;

  static_assert(std::is_base_of_v<Resource, T>, "Class must be of type resource");

  ResourcePtrT() : detail::ResourcePtrInternal(nullptr) {}

  ResourcePtrT(const ResourcePtrT &copy)
    : detail::ResourcePtrInternal(copy) {
    resource = copy.resource;
  }

  ResourcePtrT(ResourcePtrT &&move) noexcept
    : detail::ResourcePtrInternal(move), resource(move.resource) {
    move.resource = nullptr;
  }

  ~ResourcePtrT() override = default;

  ResourcePtrT &operator=(const ResourcePtrT &other) {
    if (this != &other) {
      copy_from(other);
      resource = other.resource;
    }

    return *this;
  }

  ResourcePtrT &operator=(ResourcePtrT &&other) noexcept {
    if (this != &other) {
      move_from(other);
      resource = std::move(other.resource);
    }

    return *this;
  }


  pointer get() const {
    if (!resource) {
      resource = static_cast<T *>(load());
    }

    return resource;
  }

  pointer operator*() { return get(); }

  pointer operator->() { return get(); }

private:
  explicit ResourcePtrT(ControlBlock *container)
    : detail::ResourcePtrInternal(container),
      resource(nullptr) {}

  T *resource;
};
}// namespace e00
