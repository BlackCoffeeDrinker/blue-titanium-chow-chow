#include <Engine.hpp>

#include "EngineError.hpp"
#include "InternalActions.hpp"
#include "EngineState.h"

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
                   _current_state() {
  _main_logger.Verbose(source_location::current(), "E0 Starting");
}

Engine::~Engine() = default;

detail::ControlBlock *Engine::MakeResourceContainer(const std::string &name, type_t type, const source_location &from) {
  for (const auto& a : _loaded_resources_cb) {
    if (a->type() == type && a->name() == name) {
      _main_logger.Info(source_location::current(), "Resource {} of type {} already known", name, type);
      return a.get();
    }
  }

  class R : public detail::ControlBlock {
    Engine *_owner;
    type_t _type;
    std::string _name;
    source_location _init;

  public:
    R(e00::Engine *e, e00::type_t t, std::string n, e00::source_location s)
      : _owner(e), _type(t), _name(std::move(n)), _init(s) {
      refs = 0;
      resource = nullptr;
    }

    ~R() override = default;

    [[nodiscard]] std::string name() const override { return _name; }
    [[nodiscard]] type_t type() const override { return _type; }

    void zero_refs() override { _owner->ZeroRefControlBlock(this); }

    std::error_code load() override { return _owner->LoadControlBlock(this); }
  };

  // We didn't find it so make a new container
  auto const &ret = _loaded_resources_cb.emplace_back(std::make_unique<R>(this, type, name, from));

  _main_logger.Info(source_location::current(), "New resource {} of type {} asked at {}:{}", name, type, from.file_name(), from.line());

  return ret.get();
}

void Engine::ZeroRefControlBlock(detail::ControlBlock *cb) {

}

std::error_code Engine::LoadControlBlock(detail::ControlBlock *cb) {
  if (auto stream = OpenResource(cb->name(), cb->type())) {
    // Try loading it

  }

  return impl::make_error_code(impl::EngineErrorCode::resource_not_found);
}

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
  for (const auto& [inputEvent, inputAction] : _input_binding)
    if (inputAction == action)
      return inputEvent;

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
    _main_logger.Info(source_location::current(),"Game: {}", Name());

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
  _actions_to_process.ForEach([this](const auto &action) {
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

  auto map = LazyResource<Map>(worldName);
  if (auto ec = map.EnsureLoad()) {
    _main_logger.Error(source_location::current(), "Failed to open world {}: {}", worldName, ec.message());
    return ec;
  }

  _current_world = std::make_unique<World>(worldName);
  _current_world->SetMap(std::move(map));

  // Tell the script engine that we loaded a new map



  return {};
}
}// namespace e00
