#pragma once

namespace experimental {
/**
 * Represent a source location, useful for logging
 */
struct source_location {
  static constexpr source_location current(
    const char *file = __builtin_FILE(),
    const char *func = __builtin_FUNCTION(),
    uint_least32_t line =  __builtin_LINE(),
    uint_least32_t col = 0) noexcept {
    return {file, func, line, col};
  }


  constexpr source_location() noexcept : _file("unknown"), _func(_file), _line(0), _col(0) {}
  constexpr source_location(const char *file, const char *func, uint_least32_t line, uint_least32_t col = 0) noexcept : _file(file), _func(func), _line(line), _col(col) {}

  [[nodiscard]] constexpr uint_least32_t line() const noexcept { return _line; }
  [[nodiscard]] constexpr uint_least32_t column() const noexcept { return _col; }
  [[nodiscard]] constexpr const char *file_name() const noexcept { return _file; }
  [[nodiscard]] constexpr const char *function_name() const noexcept { return _func; }

private:
  const char *_file;
  const char *_func;
  const uint_least32_t _line;
  const uint_least32_t _col;
};
}// namespace experimental

namespace e00 {
using source_location = experimental::source_location;
}
