#include <catch2/catch.hpp>
#include <Engine.hpp>

TEST_CASE("Components Can Be Added To ControlBlock") {
  e00::ComponentContainer container;
  class TestComponent : public e00::ComponentT<TestComponent> {};
  auto* c = container.CreateComponent<TestComponent>();
  REQUIRE(c != nullptr);
}

TEST_CASE("Components Can Be Retrieved From ControlBlock") {
  e00::ComponentContainer container;
  class TestComponent : public e00::ComponentT<TestComponent> {};
  auto* c = container.CreateComponent<TestComponent>();
  REQUIRE(c != nullptr);
  REQUIRE(container.GetComponent<TestComponent>() == c);
}

TEST_CASE("Components Are Deleted With ControlBlock") {
  class TestComponent : public e00::ComponentT<TestComponent> {
    int &c;

  public:
    explicit TestComponent(int &c) : c(c) { c++; }
    ~TestComponent() override { c--; }
  };

  int c = 0;

  {
    e00::ComponentContainer container;
    container.CreateComponent<TestComponent>(c);
    REQUIRE(c == 1);
  }

  REQUIRE(c == 0);
}

TEST_CASE("Components Types Are Unique Within a ControlBlock") {
  e00::ComponentContainer container;
  class TestComponent : public e00::ComponentT<TestComponent> {};

  auto* c = container.CreateComponent<TestComponent>();
  REQUIRE(c != nullptr);

  auto* c2 = container.CreateComponent<TestComponent>();
  REQUIRE(c2 == nullptr);

  REQUIRE(container.GetComponent<TestComponent>() == c);
}

TEST_CASE("Components is NULL if a type is not found") {
  e00::ComponentContainer container;
  class TestComponent : public e00::ComponentT<TestComponent> {};
  REQUIRE(container.GetComponent<TestComponent>() == nullptr);
}

