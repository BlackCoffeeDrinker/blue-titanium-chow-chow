#include "LightGameEngine.hpp"


namespace {

struct MovementBindingsCategory : public e00::ActionCategory {
  [[nodiscard]] std::string_view name() const noexcept override {
    return "Movement";
  }

  [[nodiscard]] std::string_view message(uint32_t binding) const override {
    switch (static_cast<light::MovementBindings>(binding)) {
      case light::MovementBindings::MOVE_FORWARD: return "Forward";
      case light::MovementBindings::MOVE_BACKWARDS: return "Backward";
      case light::MovementBindings::MOVE_LEFT: return "Left";
      case light::MovementBindings::MOVE_RIGHT: return "Right";
    }

    return {};
  }
};


const MovementBindingsCategory movementCategory{};
}// namespace

namespace light {
e00::Action make_action(MovementBindings e) {
  return { e, movementCategory };
}

/*
e00::Action make_action(ActionBindings e) {
}
*/

LightGameEngine::LightGameEngine()
  : Engine(), _game_log(platform::CreateSink("LightGame")) {}

std::error_code LightGameEngine::RealInit() noexcept {
  make_action(MovementBindings::MOVE_FORWARD);
  make_action(MovementBindings::MOVE_BACKWARDS);
  make_action(MovementBindings::MOVE_LEFT);
  make_action(MovementBindings::MOVE_RIGHT);

  return {};
}

std::string_view LightGameEngine::Name() const noexcept {
  return "Light";
}

void LightGameEngine::OnFirstTick() {
  _game_log.Log(e00::source_location::current(), e00::L_INFO, "First Tick");

  LoadWorld("Test");
}

std::unique_ptr<e00::Stream> LightGameEngine::OpenResource(const std::string &resourceName, e00::type_t expectedType) {
  _game_log.Log(e00::source_location::current(), e00::L_INFO, "Open world: {0}", resourceName);

  if (resourceName == "Test" && e00::type_id<e00::World>()) {
    return platform::OpenStream("Untitled-1.tmj");
  }

  return nullptr;
}

}// namespace light
