#pragma once

#include <map>
#include <utility>

#include <Engine.hpp>
#include <Engine/ResourcePtr.hpp>
#include <Engine/Logging/Logger.hpp>

#include "ResourceLoader.hpp"

namespace e00 {
class ResourceManager {
  friend class detail::ResourcePtrInternal;

  mutable Logger _logger;

  std::multimap<type_t, std::unique_ptr<ResourceLoader>> _loaders;
  std::map<uint32_t, std::unique_ptr<Resource>> _resources;

  std::function<std::unique_ptr<Stream>(const std::string&, type_t)> _stream_opener;

  struct RMContainer : public detail::ResourcePtrInternal::ControlBlock {
    type_t type;
    std::string name;
    Resource *resource;
  };

  RMContainer *make_container(const std::string &name, type_t type);

  void zero_ref(detail::ResourcePtrInternal *resource);

  Resource *load(detail::ResourcePtrInternal *resource);

  static RMContainer *container_of(detail::ResourcePtrInternal *resource) {
    return resource ? static_cast<RMContainer *>(resource->cb) : nullptr;
  }

  static const RMContainer *container_of(const detail::ResourcePtrInternal *resource) {
    return resource ? static_cast<const RMContainer *>(resource->cb) : nullptr;
  }

public:

  ResourceManager();

  ~ResourceManager();

  template<typename T>
  ResourcePtrT<T> Lazy(const std::string &name) { return { make_container(name, type_id<T>()) }; }

  void SetLoader(std::function<std::unique_ptr<Stream>(const std::string&, type_t)> func) {
    _stream_opener = std::move(func);
  }
};
}// namespace e00
