#pragma once

#include <Engine.hpp>

#include <string>
#include <system_error>
#include <memory>

namespace e00 {
class ResourceLoader {
  type_t _for_resource_id;

protected:
  Engine* _engine;

  explicit ResourceLoader(type_t type)
    : _for_resource_id(type),
      _engine(nullptr) {}

public:
  virtual ~ResourceLoader() = default;

  struct Result {
    std::error_code error;
    std::unique_ptr<Resource> resource;

    explicit Result(std::error_code ec) : error(ec), resource(nullptr) {}
    explicit Result(std::unique_ptr<Resource> &&r) : error(), resource(std::move(r)) {}
  };

  void SetResourceLoader(Engine* loader) { _engine = loader; }

  virtual bool CanLoad(const std::unique_ptr<Stream> &stream) = 0;

  virtual Result ReadLoad(const std::string &name, const std::unique_ptr<Stream> &stream) = 0;
};
}// namespace e00
