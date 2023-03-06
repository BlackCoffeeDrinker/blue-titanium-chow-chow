#pragma once

namespace e00 {
/**
 * Raw map data
 */
class Map {
public:
  using Position = Vec2D<WorldCoordinateType>;

private:
  struct TileOptions {};

  Vec2D<WorldCoordinateType> _size;
  std::vector<Tileset::TileIdType> _data;
  std::vector<TileOptions> _options;

  [[nodiscard]] WorldCoordinateType LayerSize() const { return _size.Area(); }

  [[nodiscard]] WorldCoordinateType PositionToLinear(const Position &pos) const {
    if (pos > _size) {
      return std::numeric_limits<WorldCoordinateType>::max();
    }

    return (pos.y * _size.x) + pos.x;
  }

  [[nodiscard]] bool ValidDataPosition(size_t position) const {
    return _data.size() > position;
  }

public:
  Map() : _size(0, 0), _data(), _options() {}

  Map(WorldCoordinateType width, WorldCoordinateType height)
    : _size(width, height),
      _data(LayerSize()),
      _options(LayerSize()) {}

  Map(const Map &other) = default;

  Map(Map &&other) noexcept
    : _size(other._size),
      _data(std::move(other._data)),
      _options(std::move(other._options)) {
  }

  ~Map() = default;

  Map &operator=(const Map &other) {
    if (&other != this) {
      _size = other._size;
      _data.clear();
      _data = other._data;
      _options.clear();
      _options = other._options;
    }

    return *this;
  }

  Map &operator=(Map &&other) noexcept {
    if (&other != this) {
      std::swap(_size, other._size);
      _data.clear();
      _data.swap(other._data);
      _options.clear();
      _options.swap(other._options);
    }

    return *this;
  }

  explicit operator bool() const noexcept { return _size.x > 0 && _size.y > 0; }

  [[nodiscard]] WorldCoordinateType Width() const { return _size.x; }

  [[nodiscard]] WorldCoordinateType Height() const { return _size.y; }

  [[nodiscard]] Vec2D<WorldCoordinateType> Size() const { return _size; }

  std::error_code LoadBulk(uint8_t *data);

  template<typename InputIt>
  std::error_code LoadBulk(InputIt first, InputIt last) {
    // Make sure it's the right size
    if (std::distance(first, last) != _data.size()) {
      return std::make_error_code(std::errc::invalid_argument);
    }

    // Copy the data
    auto current = first;
    auto dest = _data.begin();
    while (current != last) {
      *dest = *current;

      current++;
      dest++;
    }

    return {};
  }

  [[nodiscard]] Tileset::TileIdType HighestTitleId() const {
    return *std::max_element(std::begin(_data), std::end(_data));
  }

  /**
   * Get the Tile ID for a given position
   *
   * @param position
   * @return tileId or 0 if out of bound
   */
  [[nodiscard]] Tileset::TileIdType Get(const Position &position) const {
    const auto i = PositionToLinear(position);
    if (ValidDataPosition(i)) {
      return _data.at(i);
    }

#ifndef NDEBUG
    abort();
#endif

    return std::numeric_limits<Tileset::TileIdType>::max();
  }

  /**
   *
   *
   * @param position
   * @param tileId
   * @return
   */
  bool Set(const Position &position, Tileset::TileIdType tileId) {
    const auto i = PositionToLinear(position);
    if (ValidDataPosition(i)) {
      _data[i] = tileId;
      return true;
    }

    return false;
  }
};
}// namespace e00
