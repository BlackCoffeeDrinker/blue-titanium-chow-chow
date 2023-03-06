#pragma once

#include <string_view>
#include <type_traits>

namespace e00 {
template<typename T>
struct IsActionTypeEnum : std::false_type {};

/**
 * e00::ActionCategory serves as the base class for specific action category types
 * The objects of action category classes are treated as singletons, passed by reference.
 */
struct ActionCategory {
  constexpr ActionCategory() noexcept = default;
  ActionCategory(const ActionCategory &) = delete;
  virtual ~ActionCategory() noexcept = default;

  [[nodiscard]] virtual std::string_view name() const noexcept = 0;
  [[nodiscard]] virtual std::string_view message(uint32_t binding) const = 0;

  friend bool operator==(const ActionCategory &left, const ActionCategory &right) noexcept;
  friend bool operator!=(const ActionCategory &left, const ActionCategory &right) noexcept;
  friend bool operator<(const ActionCategory &left, const ActionCategory &right) noexcept;

  ActionCategory &operator=(const ActionCategory &) = delete;
};

inline bool operator==(const ActionCategory &left, const ActionCategory &right) noexcept {
  return &left == &right;
}

inline bool operator!=(const ActionCategory &left, const ActionCategory &right) noexcept {
  return !(left == right);
}

inline bool operator<(const ActionCategory &left, const ActionCategory &right) noexcept {
  return &left < &right;
}

/**
 * Action is a game-dependent action.
 * The action code values may be not unique across different actions categories.
 * The pair ActionCategory + Action must be unique
 */
class Action {
  // the stored binding number
  uint32_t _val;
  // pointer to binding category
  const ActionCategory *_cat;

public:
  constexpr Action() : _val(0), _cat(nullptr) {}

  template<typename T, std::enable_if_t<IsActionTypeEnum<T>::value, int> = 0>
  constexpr Action(T value, const ActionCategory &category) : _val(static_cast<uint32_t>(value)), _cat(&category) {}

  void assign(const uint32_t val, const ActionCategory &cat) noexcept {
    _val = val;
    _cat = &cat;
  }

  void clear() noexcept {
    _val = 0;
    _cat = nullptr;
  }

  [[nodiscard]] uint32_t value() const noexcept { return _val; }

  [[nodiscard]] const ActionCategory &category() const noexcept { return *_cat; }

  [[nodiscard]] std::string_view message() const { return category().message(value()); }

  explicit operator bool() const noexcept { return value() != 0; }

  friend bool operator<(const Action &left, const Action &right) noexcept {
    if (left._cat == right._cat) {
      return left._val < right._val;
    }

    return &left._cat < &right._cat;
  }

  friend bool operator==(const Action &left, const Action &right) noexcept {
    return (left.category() == right.category()) && (left.value() == right.value());
  }

  friend bool operator!=(const Action &left, const Action &right) noexcept {
    return !(left == right);
  }
};

}
