#include <catch2/catch.hpp>
#include <Engine.hpp>

class AnEngine : public e00::Engine {
public:
  explicit AnEngine(const e00::Configuration& config) : e00::Engine(config) {}

  ~AnEngine() = default;

  [[nodiscard]] std::string_view Name() const noexcept override {
    return "TestEngine";
  }

protected:
  std::error_code RealInit() override { return {}; }
};



std::unique_ptr<e00::Engine> CreateGameEngine(const e00::Configuration &configuration) {
  return std::make_unique<AnEngine>(configuration);
}

/********************************************************************************************/


TEST_CASE("Engine can be created") {
  auto engine = CreateGameEngine({});
  REQUIRE(engine != nullptr);
}

TEST_CASE("Engine can be initialized") {
  auto engine = CreateGameEngine({});
  REQUIRE(engine != nullptr);
  REQUIRE(engine->Init().value() == 0);
}

TEST_CASE("Engine can return it's name") {
  auto engine = CreateGameEngine({});
  REQUIRE(engine != nullptr);
  REQUIRE(engine->Init().value() == 0);
  REQUIRE(engine->Name() == "TestEngine");
}
