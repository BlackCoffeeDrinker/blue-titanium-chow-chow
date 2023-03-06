#pragma once

#include <Engine.hpp>

namespace light {
enum class MovementBindings : int {
  MOVE_FORWARD = 1,
  MOVE_BACKWARDS,
  MOVE_LEFT,
  MOVE_RIGHT,
};

enum class ActionBindings : int {
  USE = 1,

};

e00::Action make_action(MovementBindings e);
// e00::Action make_action(ActionBindings e);

class LightGameEngine : public e00::Engine {
  e00::Logger _game_log;

protected:
  std::error_code RealInit() noexcept override;
  void OnFirstTick() override;
  std::unique_ptr<e00::Stream> OpenResource(const std::string &resourceName, e00::type_t expectedType) override;

public:
  explicit LightGameEngine();

  ~LightGameEngine() override = default;

  std::string_view Name() const noexcept override;
};
}// namespace light

namespace e00 {
template<>
struct IsActionTypeEnum<light::MovementBindings> : std::true_type {};

template<>
struct IsActionTypeEnum<light::ActionBindings> : std::true_type {};
}// namespace e00
