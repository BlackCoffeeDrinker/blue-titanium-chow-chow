#pragma once

#include <cstdint>
#include <Engine.hpp>

#include "KeyboardEventHandlerResult.hpp"
#include "Keys.hpp"

class KeyboardEventPauseHandler {
  enum class KeyPauseSequence {
    NOT_SET,

    P_E1_INITIAL,
    P_1D,
    P_45,
    P_E1_SECOND,
    P_9D,
    P_C5
  };

  KeyPauseSequence _current_pause_sequence;
  bool _pause_key_was_pressed;

  [[nodiscard]] uint8_t expected_next_scancode() const {
    switch (_current_pause_sequence) {
      case KeyPauseSequence::P_E1_INITIAL: return 0x1D;
      case KeyPauseSequence::P_1D: return 0x45;
      case KeyPauseSequence::P_45: return 0xE1;
      case KeyPauseSequence::P_E1_SECOND: return 0x9d;
      case KeyPauseSequence::P_9D: return 0xC5;
      case KeyPauseSequence::P_C5:
      case KeyPauseSequence::NOT_SET: break;
    }
    return 0;
  }

  [[nodiscard]] KeyPauseSequence next_state() const {
    switch (_current_pause_sequence) {
      case KeyPauseSequence::P_E1_INITIAL: return KeyPauseSequence::P_1D;
      case KeyPauseSequence::P_1D: return KeyPauseSequence::P_45;
      case KeyPauseSequence::P_45: return KeyPauseSequence::P_E1_SECOND;
      case KeyPauseSequence::P_E1_SECOND: return KeyPauseSequence::P_9D;
      case KeyPauseSequence::P_9D: return KeyPauseSequence::P_C5;
      case KeyPauseSequence::P_C5:
      case KeyPauseSequence::NOT_SET: break;
    }

    return KeyPauseSequence::NOT_SET;
  }

public:
  KeyboardEventPauseHandler()
    : _current_pause_sequence(KeyPauseSequence::NOT_SET),
      _pause_key_was_pressed(false) {}

  [[nodiscard]] bool is_active() const { return _current_pause_sequence != KeyPauseSequence::NOT_SET; }

  KeyboardEventHandlerResult handle(uint8_t scancode) {
    // handle the multiple codes generated by the pause key
    if (is_active()) {
      // make sure we're going through the right sequence
      if (scancode == expected_next_scancode()) {
        _current_pause_sequence = next_state();
        return KeyboardEventHandlerResult(false);
      }

      _current_pause_sequence = KeyPauseSequence::NOT_SET;
    }

    // Check for pause key
    // Pause is E1 1D 45 E1 9D C5  (not typematic)
    if (scancode == 0xE1) {
      // Reset the state machine
      _current_pause_sequence = KeyPauseSequence::P_E1_INITIAL;
      _pause_key_was_pressed = !_pause_key_was_pressed;

      if (_pause_key_was_pressed) {
        // PAUSE KEY RELEASED
        return KeyboardEventHandlerResult(e00::InputEvent(e00::InputEvent::Type::Release, KeyboardInputSystem(), KEY_PAUSE, 0));
      }

      // PAUSE KEY PRESSED
      return KeyboardEventHandlerResult(e00::InputEvent(e00::InputEvent::Type::Press, KeyboardInputSystem(), KEY_PAUSE, 0));
    }

    return KeyboardEventHandlerResult(true);
  }
};