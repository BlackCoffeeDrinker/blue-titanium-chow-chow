#pragma once

namespace e00 {
template<typename T>
struct Vec2D {
  T x, y;

  constexpr Vec2D() noexcept : x{}, y{} {}
  constexpr Vec2D(const Vec2D &other) noexcept : x(other.x), y(other.y) {}
  constexpr Vec2D(T x_, T y_) noexcept : x{ x_ }, y{ y_ } {}

  constexpr auto Area() const { return x * y; }

  constexpr Vec2D<T> Clamp(const Vec2D<T> &maximums) const {
    return {
      x > maximums.x ? maximums.x : x,
      y > maximums.y ? maximums.y : y
    };
  }

  constexpr bool operator==(const Vec2D<T> &rhs) const { return x == rhs.x && y == rhs.y; }
  constexpr bool operator!=(const Vec2D<T> &rhs) const { return x != rhs.x || y != rhs.y; }

  constexpr bool operator>(const Vec2D<T> &rhs) const { return x > rhs.x && y > rhs.y; }
  constexpr bool operator<(const Vec2D<T> &rhs) const { return x < rhs.x && y < rhs.y; }

  constexpr Vec2D<T> operator+(const Vec2D<T> &rhs) const { return Vec2D<T>(x + rhs.x, y + rhs.y); }
  constexpr Vec2D<T> operator-(const Vec2D<T> &rhs) const { return Vec2D<T>(x - rhs.x, y - rhs.y); }
  constexpr Vec2D<T> operator*(const Vec2D<T> &rhs) const { return Vec2D<T>(x * rhs.x, y * rhs.y); }
  constexpr Vec2D<T> operator/(const Vec2D<T> &rhs) const { return Vec2D<T>(x / rhs.x, y / rhs.y); }

  constexpr Vec2D<T> operator+(const T &rhs) const { return Vec2D<T>(x + rhs, y + rhs); }
  constexpr Vec2D<T> operator-(const T &rhs) const { return Vec2D<T>(x - rhs, y - rhs); }
  constexpr Vec2D<T> operator*(const T &rhs) const { return Vec2D<T>(x * rhs, y * rhs); }
  constexpr Vec2D<T> operator/(const T &rhs) const { return Vec2D<T>(x / rhs, y / rhs); }
};

template<typename T>
constexpr Vec2D<T> min(const Vec2D<T>& lhs, const Vec2D<T>& rhs) {
  return {
    lhs.x < rhs.x ? lhs.x : rhs.x,
    lhs.y < rhs.y ? lhs.y : rhs.y
  };
}

template<typename T>
constexpr Vec2D<T> max(const Vec2D<T>& lhs, const Vec2D<T>& rhs) {
  return {
    lhs.x > rhs.x ? lhs.x : rhs.x,
    lhs.y > rhs.y ? lhs.y : rhs.y
  };
}

template<typename T>
struct AABB {
  Vec2D<T> minimum;
  Vec2D<T> maximum;
};

template<typename T>
AABB<T> merge(const AABB<T>& lhs, const AABB<T>& rhs) {
  return {
    min(lhs.minimum, rhs.minimum),
    max(lhs.maximum, rhs.maximum)
  };
}

template<typename T>
struct RectT {
  Vec2D<T> origin;
  Vec2D<T> size;

  constexpr RectT() noexcept : origin{}, size{} {}
  constexpr RectT(const RectT &other) noexcept : origin(other.origin), size(other.size) {}
  constexpr RectT(T x, T y, T width, T height) noexcept : origin(x, y), size(width, height) {}
  constexpr RectT(const Vec2D<T> &pos, const Vec2D<T> &size) noexcept : origin(pos), size(size) {}

  constexpr Vec2D<T> From() const { return origin; }
  constexpr Vec2D<T> To() const { return Vec2D<T>(origin.x + size.x, origin.y + size.y); }

  constexpr Vec2D<T> GetTopLeft() const { return From(); }
  constexpr Vec2D<T> GetTopRight() const { return {origin.x + size.x, origin.y}; }
  constexpr Vec2D<T> GetBottomLeft() const { return {origin.x, origin.y + size.y}; }
  constexpr Vec2D<T> GetBottomRight() const { return To(); }

  constexpr Vec2D<T> Center() const {
    return {
      (origin.x + size.x) / static_cast<T>(2),
      (origin.y + size.y) / static_cast<T>(2)
    };
  }

  constexpr bool Contains(const Vec2D<T> &point) const {
    return (point.x >= origin.x && point.y >= origin.y
            && point.x < origin.x + size.x && point.y < origin.y + size.y);
  }

  constexpr bool Contains(const RectT<T> &r2) const {
    return (origin.x < r2.origin.x + r2.size.x && origin.x + size.x > r2.origin.x)
           && (origin.y < r2.origin.y + r2.size.y && origin.y + size.y > r2.origin.y);
  }


  constexpr static RectT FromPositions(const Vec2D<T> &from, const Vec2D<T> &to) {
    RectT<T> r;
    if (to.x > from.x) {
      r.origin.x = from.x;
      r.size.width = to.x - from.x;
    } else {
      r.origin.x = to.x;
      r.size.width = from.x - to.x;
    }

    if (to.y > from.y) {
      r.origin.y = from.y;
      r.size.height = to.y - from.y;
    } else {
      r.origin.y = to.y;
      r.size.height = from.y - to.y;
    }

    return r;
  }
};

template<typename T>
struct Ray2D {
  Vec2D<T> origin;
  Vec2D<T> direction;

  struct ContactResult {
    bool intersects;
    Vec2D<T> contact_point;

    explicit operator bool() const { return intersects; }
  };
  /*
    ContactResult Intersects(const RectT<T>& rect) const {
      T near = (rect.origin - origin) / direction;
      T far = (rect.to() - origin) / direction;

      if (near.x > far.x) std::swap(near.x, far.x);
      if (near.y > far.y) std::swap(near.y, far.y);

      T hit_near = std::max(near.x, near.y);
      T hit_far = std::min(far.x, far.y);

      if (hit_far < 0) return { false, {} };
      ContactResult result;
      result.intersects = true;
      //result.contact_point =
      return result;
    }*/
};


struct ColorHSV final {
  uint8_t hue;// this uses 0-255 and not 0-360 as commonly found for hue
  uint8_t saturation;
  uint8_t value;

  constexpr ColorHSV() : hue(0), saturation(0), value(0) {}

  ColorHSV(uint8_t h, uint8_t s, uint8_t v) : hue(h), saturation(s), value(v) {}

  ColorHSV(const ColorHSV &other) = default;
};

struct Color final {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  static constexpr Color fromRGB(uint32_t rgb) {
    return Color(
      ((rgb & 0x00FF0000u) >> 16u) & 0xFF,
      ((rgb & 0x0000FF00u) >> 8u) & 0xFF,
      (rgb & 0x000000FFu));
  }

  static constexpr Color fromBGR(uint32_t bgr) {
    return Color(
      ((bgr & 0x00FF0000u) >> 0u) & 0xFF,
      ((bgr & 0x0000FF00u) >> 8u) & 0xFF,
      ((bgr & 0x000000FFu) >> 16u) & 0xFF);
  }

  constexpr Color() : red(0), green(0), blue(0) {}

  constexpr Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

  Color &operator=(const Color &rhs) {
    if (&rhs != this) {
      red = rhs.red;
      green = rhs.green;
      blue = rhs.blue;
    }
    return *this;
  }

  void set(uint8_t r, uint8_t g, uint8_t b) noexcept {
    this->red = r;
    this->green = g;
    this->blue = b;
  }

  [[nodiscard]] uint8_t get_red() const noexcept { return red; }
  [[nodiscard]] uint8_t get_green() const noexcept { return green; }
  [[nodiscard]] uint8_t get_blue() const noexcept { return blue; }

  [[nodiscard]] uint8_t get_rgb_min() const noexcept {
    return red < green ? (red < blue ? red : blue) : (green < blue ? green : blue);
  }

  [[nodiscard]] uint8_t get_rgb_max() const noexcept {
    return red > green ? (red > blue ? red : blue) : (green > blue ? green : blue);
  }

  /**
   * Luma value, CCIR 601 weighted
   *
   * @return 0 to 255000
   */
  [[nodiscard]] uint32_t luma() const noexcept {
    return red * 299u + green * 587u + blue * 114u;
  }

  explicit operator uint32_t() const noexcept {
    return static_cast<uint32_t>((red << 16u) + (green << 8u) + blue);
  }

  [[nodiscard]] uint32_t toBGR() const noexcept {
    return static_cast<uint32_t>((red << 16u) + (green << 8u) + blue);
  }

  bool operator==(const Color &rhs) const noexcept {
    return (red == rhs.red)
           && (green == rhs.green)
           && (blue == rhs.blue);
  }
};

// TODO
class Matrix {
  int32_t v[3][3]; /* scaling and rotation */
  int32_t t[3]; /* translation */
};

}// namespace e00
