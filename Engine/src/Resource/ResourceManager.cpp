
#include "ResourceManager.hpp"
#include "Engine/ResourcePtr.hpp"

namespace e00 {
ResourceManager::ResourceManager()
  : _logger(platform::CreateSink("ResourceManager")) {
}

ResourceManager::~ResourceManager() {
  // Unload all resources
  for (auto i = _loaded_resources_cb.begin(); i != _loaded_resources_cb.end(); i++) {
    unload((*i).get());
  }

  _loaded_resources_cb.clear();
}

ResourceManager::RMContainer *ResourceManager::make_container(const std::string &name, type_t type, const source_location &from) {
  // TODO: make this better
  for (auto i = _loaded_resources_cb.begin(); i != _loaded_resources_cb.end(); i++) {
    if ((*i)->_type == type && (*i)->_name == name) {
      _logger.Info(source_location::current(), "Resource {} of type {} already known", name, type);
      return (*i).get();
    }
  }

  // We didn't find it so make a new container
  auto &ret = _loaded_resources_cb.emplace_back(std::make_unique<RMContainer>());

  ret->refs = 0;
  ret->_owner = this;
  ret->_type = type;
  ret->_name = name;
  ret->_init = from;

  _logger.Info(source_location::current(), "New resource {} of type {} asked at {}:{}", name, type, from.file_name(), from.line());

  return ret.get();
}

void ResourceManager::zero_ref(RMContainer *rm_container) {
  if (!rm_container) {
    _logger.Error(source_location::current(), "ZeroRef null container");
    return;
  }

  unload(rm_container);

  for (auto it = _loaded_resources_cb.begin(); it != _loaded_resources_cb.end(); it++) {
    if ((*it).get() != rm_container)
      continue;

    // Hint the static-analysis that rm_container is going away
    rm_container = nullptr;

    (*it).reset();
    _loaded_resources_cb.erase(it);
    return;
  }

  // This shouldn't happen
  _logger.Error(source_location::current(), "Resource {} of type {} was not found in known resources !", rm_container->_name, rm_container->_type);

#ifndef DEBUG
  abort();
#endif
}

Resource *ResourceManager::load(RMContainer *rm_container) {
  // Check if the resource is loaded ?
  // This shouldn't happen, but it's better to be safe than sorry
  if (rm_container->resource) {
    _logger.Error(source_location::current(), "Resource {} was already loaded", rm_container->_name);
    return rm_container->resource;
  }

  // Load it
  _logger.Info(source_location::current(), "Loading resource {}", rm_container->_name);

  // Make sure we have a stream
  auto stream = _stream_opener(rm_container->_name, rm_container->_type);
  if (!stream) {
    _logger.Error(source_location::current(), "Resource {} ({}) source was not found", rm_container->name(), rm_container->_type);
    return nullptr;
  }

  // Find loader for type
  for (auto it = _loaders.find(rm_container->_type); it != _loaders.end(); it++) {
    stream->seek(0);

    // This loader claims it can handle this type, can it load this stream ?
    if (!it->second->CanLoad(stream)) {
      // Reset stream and try the next loader
      continue;
    }

    // We found a loader so inform everyone
    _logger.Verbose(source_location::current(), "Found a resources loader {}", rm_container->_name);

    // Read it
    auto ret = it->second->ReadLoad(rm_container->_name, stream);
    if (ret.error) {
      // Something happened and wasn't loaded
      _logger.Error(source_location::current(), "Resource {} ({}) failed to load: {}", rm_container->name(), rm_container->_type, ret.error.message());
      continue;
    }

    return ret.resource.release();
  }

  return nullptr;
}

void ResourceManager::unload(ResourceManager::RMContainer *rm_container) {
  if (!rm_container->resource)
    return;

  // Unload resource
  _logger.Verbose(source_location::current(),
    "Unloading resource {} (loaded at {}:{})",
    rm_container->_name,
    rm_container->_init.file_name(),
    rm_container->_init.line());

  delete rm_container->resource;
  rm_container->resource = nullptr;
}

std::error_code ResourceManager::AddLoader(type_t type, std::unique_ptr<ResourceLoader> &&loader) {
  auto i = _loaders.insert(std::make_pair(type, std::move(loader)));
  i->second->SetResourceLoader(this);

  // TODO
  return {};
}
}// namespace e00
