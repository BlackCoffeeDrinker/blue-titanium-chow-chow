#pragma once

#include <Engine.hpp>

#include <string>
#include <system_error>
#include <memory>

namespace e00 {
class ResourceManager;

class ResourceLoader {
  type_t _for_resource_id;

protected:
  ResourceManager *_resource_manager;

  explicit ResourceLoader(type_t type)
    : _for_resource_id(type),
      _resource_manager(nullptr) {}

public:
  virtual ~ResourceLoader() = default;

  struct Result {
    std::error_code error;
    std::unique_ptr<Resource> resource;

    explicit Result(std::error_code ec) : error(ec), resource(nullptr) {}
    explicit Result(std::unique_ptr<Resource> &&r) : error(), resource(std::move(r)) {}
  };

  void SetResourceLoader(ResourceManager* loader) { _resource_manager = loader; }

  virtual bool CanLoad(const std::unique_ptr<Stream> &stream) = 0;

  virtual Result ReadLoad(const std::string &name, const std::unique_ptr<Stream> &stream) = 0;
};
}// namespace e00
