#include <Engine.hpp>

#include "EngineError.hpp"
#include "InternalActions.hpp"
#include "EngineState.h"

#include "Resource/ResourceManager.hpp"

#include "WorldLoader.hpp"

namespace e00 {
Action Engine::BuiltInAction_Quit() {
  return impl::make_action(impl::EngineAction::Quit);
}

Engine::Engine() : _main_logger(platform::CreateSink("Engine")),
                   _initialized(false),
                   _needToQuit(true),
                   _flagAfterInit(true),
                   _current_time(0),
                   _script_engine(ScriptEngine::Create()),
                   _resources(std::make_unique<ResourceManager>()),
                   _current_state() {
  _main_logger.Log(source_location::current(), L_VERBOSE, "E0 Starting");
  _resources->SetStreamOpener([&](const auto &a1, auto a2) { return OpenResource(a1, a2); });
  _resources->AddLoader<Map, impl::WorldLoader>();
}

Engine::~Engine() = default;

std::error_code Engine::AddActionBinding(std::unique_ptr<Binding> &&bindingToAdd) {
  _action_binding.erase(bindingToAdd->action());
  _action_binding.insert(std::make_pair(bindingToAdd->action(), std::move(bindingToAdd)));

  return {};
}

std::vector<Action> Engine::Actions() const noexcept {
  std::vector<Action> return_vector;
  return_vector.reserve(_action_binding.size());

  for (const auto &i : _action_binding)
    return_vector.push_back(i.first);

  return return_vector;
}

InputEvent Engine::InputBindingForAction(const Action &action) const noexcept {
  for (const auto &i : _input_binding)
    if (i.second == action)
      return i.first;

  return {};
}

std::error_code Engine::BindInputEventToAction(const Action &action, InputEvent event) noexcept {
  _input_binding.erase(event);
  _input_binding.insert(std::make_pair(event, action));

  return {};
}

bool Engine::IsRunning() const noexcept {
  return _initialized && !_needToQuit;
}

void Engine::Tick(const std::chrono::milliseconds &delta) noexcept {
  if (!_flagAfterInit) {
    OnFirstTick();
    _flagAfterInit = true;
  }

  // Make sure we're running!
  if (_needToQuit) {
    return;
  }

  // Update engine time
  _current_time += delta;

  // Process actions
  _actions_to_process.ForEach([&](const auto &action) {
    // If we do have an action for it, is there a global binding for it ?
    if (const auto it = _action_binding.find(action.action);
        it != _action_binding.end()) {
      // do it
      it->second->run();
      return;
    }

    // Send action to the current state
    if (_current_state) {
      _current_state->ExecuteAction(action);
    }
  });

  // Inform the subsystems we should tick
  if (_current_state) {
    _current_state->Tick(delta);
  }
}

void Engine::Draw() noexcept {
}

std::error_code Engine::Init() noexcept {
  if (!_initialized) {
    if (auto ec = RealInit()) {
      _needToQuit = true;
      return ec;
    }
    _initialized = true;
    _needToQuit = false;
    _flagAfterInit = false;
  }

  return {};
}

std::error_code Engine::SetOutputScreen(Bitmap *screen) noexcept {
  // Set the new reference

  // Update caches

  return {};
}

std::error_code Engine::LoadWorld(const std::string &worldName) {
  _main_logger.Info(source_location::current(), "Opening world {}", worldName);

  World world(worldName);
  world.SetMap(_resources->Lazy<Map>(worldName));

  _main_logger.Error(source_location::current(), "Failed to open world {}", worldName);
  return impl::make_error_code(impl::EngineErrorCode::level_not_found);
}
}// namespace e00
