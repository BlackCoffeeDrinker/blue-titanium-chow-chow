#pragma once

#include <Engine.hpp>

namespace e00 {
class Engine::State {
  bool _ticked;
  Engine *_engine;

protected:
  virtual void OnEnter() noexcept = 0;

  virtual void Process(const std::chrono::milliseconds &delta) noexcept = 0;

public:
  State() : _ticked(false), _engine(nullptr) {}

  virtual ~State() = default;

  void SetEngine(Engine *e) { _engine = e; }

  virtual void ExecuteAction(const Engine::ActionInstance& action) = 0;

  void Tick(const std::chrono::milliseconds &delta) noexcept {
    if (!_ticked) {
      OnEnter();
      _ticked = true;
    }

    Process(delta);
  }
};
}
