#pragma once

#include <Engine.hpp>

#include <string>
#include <system_error>

namespace e00 {
class ResourceLoader {
  type_t _for_resource_id;

protected:
  explicit ResourceLoader(type_t type) : _for_resource_id(type) {}

public:
  virtual ~ResourceLoader() = default;

  virtual bool CanLoad(const std::unique_ptr<Stream>& stream) = 0;

  virtual std::error_code ReadLoad(Resource& resource_ptr, const std::string& name, const std::unique_ptr<Stream>& stream) = 0;
};


}// namespace e00
