
#include "ResourceManager.hpp"
#include "Engine/ResourcePtr.hpp"

namespace e00 {
Resource *detail::ResourcePtrInternal::load() {
  if (cb) {
    return cb->owner->load(this);
  }

  return nullptr;
}

void detail::ResourcePtrInternal::add_ref() {
  if (cb) {
    cb->refs++;
  }
}

void detail::ResourcePtrInternal::remove_ref() {
  if (cb) {
    cb->refs--;
    if (cb->refs == 0) {
      cb->owner->zero_ref(this);
    }
  }
}

std::string detail::ResourcePtrInternal::Name() const {
  if (auto rm_container = ResourceManager::container_of(this)) {
    return rm_container->name;
  }

  return {};
}

ResourceManager::ResourceManager()
  : _logger(platform::CreateSink("ResourceManager")) {
}

ResourceManager::~ResourceManager() {
}

ResourceManager::RMContainer *ResourceManager::make_container(const std::string &name, type_t type) {
  auto *ret = new RMContainer();

  ret->owner = this;
  ret->refs = 0;
  ret->type = type;
  ret->name = name;

  return ret;
}

void ResourceManager::zero_ref(detail::ResourcePtrInternal *resource) {
  if (auto rm_container = container_of(resource)) {
    if (rm_container->resource) {
      // Unload resource
    }
  }
}
Resource *ResourceManager::load(detail::ResourcePtrInternal *resource) {
  // Is the resource valid?
  if (auto rm_container = container_of(resource)) {
    // Check if the resource is still loaded ?
    if (rm_container->resource) {
      return rm_container->resource;
    }

    // Load it
    _logger.Log(experimental::source_location::current(), L_INFO, "Loading resource {}", rm_container->name);

    if (const auto it = _loaders.find(rm_container->type);
        it != _loaders.end()) {
      // We have a loader for this! Let's try loading it
      _logger.Log(experimental::source_location::current(), L_VERBOSE, "Found a resources loader {}", rm_container->name);
      if (auto stream = _stream_opener(rm_container->name, rm_container->type)) {
        // We have a stream

      }
    }
  }

  return nullptr;
}
}// namespace e00
