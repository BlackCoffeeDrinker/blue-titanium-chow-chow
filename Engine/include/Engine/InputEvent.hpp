#pragma once

#include <string>

namespace e00 {

/**
 *
 */
struct InputSystem {
  /**
   * obtains the name of the input system
   * (eg. Mouse, Keyboard...)
   *
   * @return a string specifying the name of the input system.
   */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * Returns a string describing the given error condition for the
   * input event represented by *this
   *
   * @param value specifies the input event to describe
   * @return A string describing the given input event.
   */
  [[nodiscard]] virtual std::string name(uint16_t value) const = 0;
};

const InputSystem &UnknownInputSystem() noexcept;

/**
 *
 */
class InputEvent {
public:
  enum class Type : uint8_t {
    None = 0,
    Press,
    Release,
    Axis,
  };

private:
  Type _type;
  const InputSystem *_input_system;
  uint16_t _value;
  int16_t _axis_delta = 0;

public:
  InputEvent() : _type(Type::None), _input_system(&UnknownInputSystem()), _value(0) {}

  InputEvent(Type action_, const InputSystem &source_, uint16_t event_id_, int16_t delta_)
    : _type(action_), _input_system(&source_), _value(event_id_), _axis_delta(delta_) {}

  InputEvent(Type action_, const InputSystem &source_, uint16_t event_id_)
    : _type(action_), _input_system(&source_), _value(event_id_) {}

  explicit operator bool() const noexcept { return _type != Type::None; }

  friend bool operator==(const InputEvent &left, const InputEvent &right) noexcept {
    return (left._input_system == right._input_system)
           && (left._value == right._value)
           && (left._type == right._type);
  }

  friend bool operator!=(const InputEvent &left, const InputEvent &right) noexcept {
    return (left._input_system != right._input_system)
           || (left._value != right._value)
           || (left._type != right._type);
  }

  void assign(Type type, uint16_t value, const InputSystem &system) {
    _type = type;
    _value = value;
    _input_system = &system;
  }

  void clear() {
    assign(Type::None, 0, UnknownInputSystem());
  }

  [[nodiscard]] auto type() const noexcept { return _type; }

  [[nodiscard]] auto id() const noexcept { return _value; }

  [[nodiscard]] auto axis_delta() const noexcept { return _axis_delta; }

  [[nodiscard]] const InputSystem &input_system() const noexcept { return *_input_system; }

  [[nodiscard]] auto message() const noexcept { return input_system().name(_value); }

  friend bool operator<(const InputEvent &left, const InputEvent &right) noexcept {
    return left.hash() < right.hash();
  }

  [[nodiscard]] size_t hash() const noexcept {
    const size_t part1 = (static_cast<uint8_t>(_type) << 24);
    const size_t part2 = reinterpret_cast<intptr_t>(_input_system) << 16;
    const size_t part3 = _value;
    return part1 | part2 | part3;
  }
};

namespace helper::input {
  // Use directly and
  template<uint16_t axis, uint16_t offset = 200>
  struct AxisHelper {
    const InputSystem &source;
    int16_t delta = 0;

    [[nodiscard]] InputEvent GetAndReset() {
      InputEvent event(InputEvent::Type::Axis, source, offset + axis, delta);
      delta = 0;
      return event;
    }

    void add(int16_t d) { delta = delta + d; }

    explicit operator bool() const { return delta != 0; }
  };

  template<uint16_t button_id, uint16_t offset = 500>
  struct ButtonHelper {
    const InputSystem &source;
    bool has_data = false;
    InputEvent::Type type = InputEvent::Type::Release;

    void set(InputEvent::Type set_type) {
      has_data = true;
      type = set_type;
    }

    [[nodiscard]] InputEvent GetAndReset() {
      has_data = false;
      return { type, source, offset + button_id, 0 };
    }

    explicit operator bool() const { return has_data; }
  };
}// namespace helper::input
}// namespace e00

namespace std {
template<>
struct hash<e00::InputEvent> {
  size_t operator()(e00::InputEvent event) const noexcept {
    return event.hash();
  }
};
}// namespace std
