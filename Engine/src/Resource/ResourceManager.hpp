#pragma once

#include <map>
#include <utility>
#include <list>

#include <Engine.hpp>
#include <Engine/ResourcePtr.hpp>
#include <Engine/Logging/Logger.hpp>

#include "ResourceLoader.hpp"

namespace e00 {
class ResourceManager {
  struct RMContainer : public detail::ControlBlock {
    ResourceManager *_owner;
    type_t _type;
    std::string _name;
    source_location _init;

    std::string name() const override { return _name; }

    void zero_refs() override { _owner->zero_ref(this); }

    void load() override {
      if (!resource)
        resource = _owner->load(this);
    }
  };

  mutable Logger _logger;

  std::multimap<type_t, std::unique_ptr<ResourceLoader>> _loaders;
  std::list<std::unique_ptr<RMContainer>> _loaded_resources_cb;// << TODO: Find better container
  std::function<std::unique_ptr<Stream>(const std::string &, type_t)> _stream_opener;

  RMContainer *make_container(const std::string &name, type_t type, const source_location &from);

  void zero_ref(RMContainer *);

  Resource *load(RMContainer *);

  void unload(RMContainer *);

  std::error_code AddLoader(type_t type, std::unique_ptr<ResourceLoader> &&);

public:
  ResourceManager();

  ~ResourceManager();

  template<typename T>
  ResourcePtrT<T> Lazy(const std::string &name, const source_location &from = source_location::current()) {
    return ResourcePtrT<T>(make_container(name, type_id<T>(), from));
  }

  template<typename T, typename Z, typename... Args>
  std::error_code AddLoader(Args... args) {
    static_assert(std::is_base_of_v<Resource, T>, "Class must be of type resource");
    return AddLoader(type_id<T>(), std::make_unique<Z>(std::forward<Args>(args)...));
  }

  void SetStreamOpener(std::function<std::unique_ptr<Stream>(const std::string &, type_t)> func) {
    _stream_opener = std::move(func);
  }
};
}// namespace e00
