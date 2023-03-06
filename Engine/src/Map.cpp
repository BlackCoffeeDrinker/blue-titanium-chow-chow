#include <Engine.hpp>

namespace e00 {
std::error_code Map::LoadBulk(uint8_t *data) {
  std::copy(data, data + _data.size(), _data.begin());

  return {};
}


}// namespace e00
