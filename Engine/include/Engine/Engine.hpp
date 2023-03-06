#pragma once

/*
 * Includes are here to keep the static analysis tool happy
 */
#include <chrono>
#include <map>
#include <memory>
#include <system_error>

#include "Detail/CircularBuffer.hpp"

#include "Action.hpp"
#include "Binding.hpp"
#include "InputEvent.hpp"

#include "Scripting/ScriptEngine.hpp"

namespace e00 {
class ResourceManager;

struct Configuration {
};

/**
 * Root engine class, you should subclass this for your game logic
 */
class Engine {
  Logger _main_logger;

  struct ActionInstance {
    Action action;
    std::chrono::milliseconds when;

    constexpr ActionInstance() : action(), when(0) {}

    constexpr ActionInstance(Action action, std::chrono::milliseconds mili) : action(action), when(mili) {}
  };

  class State;
  friend State;

  bool _initialized;
  bool _needToQuit;
  bool _flagAfterInit;

  // Current game time
  std::chrono::milliseconds _current_time;

  // Actions to be executed next tick
  detail::CircularBuffer<ActionInstance, 254> _actions_to_process;

  // Action to their bindings
  std::map<Action, std::unique_ptr<Binding>> _action_binding;

  // Input event to actions
  std::map<InputEvent, Action> _input_binding;

  // Persistent script engine
  std::unique_ptr<ScriptEngine> _script_engine;

  // Resource manager
  std::unique_ptr<ResourceManager> _resources;

  // Current state of the Engine
  std::unique_ptr<State> _current_state;

  std::error_code AddActionBinding(std::unique_ptr<Binding> &&bindingToAdd);

protected:
  explicit Engine();

  /**
   *
   * @tparam Fn
   * @param action
   * @param function_to_call
   * @return error, if any otherwise the action will be added
   */
  template<typename Fn>
  std::error_code AddBindableAction(const Action action, Fn &&function_to_call) {
    return AddActionBinding(make_binding(action, std::forward<Fn>(function_to_call)));
  }

  virtual std::error_code RealInit() = 0;

  /**
   * Called only once
   */
  virtual void OnFirstTick() {
    /* Nothing needs to be done here, this is left for subclasses */
  }

  virtual std::unique_ptr<Stream> OpenResource(const std::string &resourceName, type_t expectedType) = 0;

public:
  /**
   *
   * @return the built in "Quit" action
   */
  static Action BuiltInAction_Quit();

  /**
   *
   */
  virtual ~Engine();

  /**
   * Must be called by the platform code after
   *
   * @return any error code
   */
  std::error_code Init() noexcept;

  /**
   * Sets the output screen
   * @param screen output bitmap
   * @return errors, if any
   */
  std::error_code SetOutputScreen(Bitmap *screen) noexcept;

  /**
   * Returns the code name of the game
   *
   * @return the code name of the game
   */
  [[nodiscard]] virtual std::string_view Name() const noexcept = 0;

  /**
   * Makes a list of all known actions and returns it
   *
   * @return a vector with all known actions
   */
  [[nodiscard]] std::vector<Action> Actions() const noexcept;

  /**
   * Retrieves the associated input bound to this action
   *
   * @param action the action to query
   * @return the input event associated with this action
   */
  [[nodiscard]] InputEvent InputBindingForAction(const Action &action) const noexcept;

  /**
   * Binds an InputEvent to an Action
   *
   * @param action the target action
   * @param event the input event associated
   * @return error, if any
   */
  std::error_code BindInputEventToAction(const Action &action, InputEvent event) noexcept;

  /**
   * Queries if this engine instance is valid
   *
   * @return true if this engine instance is valid
   */
  [[nodiscard]] bool IsRunning() const noexcept;

  /**
   * Processes a delta tick
   *
   * @param delta time since last tick
   */
  void Tick(const std::chrono::milliseconds &delta) noexcept;

  /**
   * Draw the current state to the screen
   */
  void Draw() noexcept;

  /**
   * Informs this instance's engine that an Input has been received
   *
   * @param event the event
   */
  void ProcessInputEvent(const InputEvent event) {
    const auto &event_binding = _input_binding.find(event);
    if (event_binding != _input_binding.end()) {
      ExecuteAction(event_binding->second);
    }
  }

  /**
   * Unconditionally execute this action on the next tick
   *
   * @param action the action
   */
  void ExecuteAction(const Action action) {
    _actions_to_process.Insert({ action, _current_time });
  }

  /**
   * Loads a world into this engine and jumps to it
   *
   * @param worldName
   * @return
   */
  std::error_code LoadWorld(const std::string &worldName);
};
}// namespace e00
