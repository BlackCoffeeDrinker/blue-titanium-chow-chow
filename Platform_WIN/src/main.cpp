#include <Windows.h>
#include <memory>
#include <string>
#include <iostream>

#include <Engine.hpp>
#include <utility>
#include <SDL.h>

#include "StdFile.hpp"

namespace {
const auto OPTIMAL_RENDER_DELAY = std::chrono::milliseconds(32);

class SDL_KeyboardSystem : public e00::InputSystem {
public:
  [[nodiscard]] std::string name() const override {
    return "Keyboard";
  }

  [[nodiscard]] std::string name(uint16_t value) const override {
    return { SDL_GetKeyName(value) };
  }
};

class SDLBitmap : public e00::Bitmap {
  SDL_Surface *_surface;

public:
  explicit SDLBitmap(SDL_Surface *fromSurface) : _surface(fromSurface) {
  }

  SDLBitmap(int w, int h)
    : _surface(SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0)) {
  }

  ~SDLBitmap() override {
    SDL_FreeSurface(_surface);
  }

  [[nodiscard]] auto Surface() const { return _surface; }

  [[nodiscard]] e00::Vec2D<uint16_t> Size() const override {
    if (_surface) {
      return {
        static_cast<uint16_t>(_surface->w),
        static_cast<uint16_t>(_surface->h)
      };
    }

    return {};
  }

  bool HasPalette() const override {
    return false;
  }
  
  std::error_code SetPalette(const std::array<e00::Color, 255> &palette) override {
    return {};
  }

  e00::Color GetPaletteColor(uint8_t index) const override {
    return {};
  }
};

const SDL_KeyboardSystem keyboard_system{};

e00::InputEvent MakeSDLKey(const SDL_KeyboardEvent &key_event) {
  auto type = e00::InputEvent::Type::None;
  if (key_event.type == SDL_KEYUP) {
    type = e00::InputEvent::Type::Release;
  } else if (key_event.type == SDL_KEYDOWN) {
    type = e00::InputEvent::Type::Press;
  }

  return { type, keyboard_system, static_cast<uint16_t>(key_event.keysym.sym & 0xFFFF) };
}

}// namespace

namespace platform {
std::unique_ptr<e00::Stream> OpenStream(const std::string &name) {
  return StdFile::CreateFromFilename(name);
}

std::unique_ptr<e00::LoggerSink> CreateSink(const std::string& name) {
class Sink : public e00::LoggerSink {
    std::string _name;
  public:
    explicit Sink(std::string  name) : _name(std::move(name)) {

    }

    void log(const e00::detail::LogMessage &msg) override {
      fprintf(stderr, "[%s] %s\n", _name.c_str(), msg.payload.data());
      fflush(stderr);
    }

    void flush() override {
      fflush(stderr);
    }
};

  return std::make_unique<Sink>(name);
}
}

static void MainLoop(const std::unique_ptr<e00::Engine> &engine, SDL_Window *sdlWindow) {
  auto oldTime = std::chrono::steady_clock::now();
  auto lastRenderTime = std::chrono::steady_clock::now();
  bool hadFocus = true;

  while (engine->IsRunning()) {
    auto nowPoint = std::chrono::steady_clock::now();
    const auto endPoint = nowPoint + OPTIMAL_RENDER_DELAY;

    // Compute delta
    const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(nowPoint - oldTime);
    engine->Tick(delta);
    oldTime = nowPoint;

    // Event loop
    while (std::chrono::steady_clock::now() < endPoint) {
      SDL_Event e;
      if (SDL_PollEvent(&e) != 0) {
        const auto eventType = static_cast<SDL_EventType>(e.type);
        switch (eventType) {
          default: break;

          case SDL_QUIT:
            engine->ExecuteAction(e00::Engine::BuiltInAction_Quit());
            break;

          case SDL_WINDOWEVENT:
            switch (static_cast<SDL_WindowEventID>(e.window.event)) {
              case SDL_WINDOWEVENT_FOCUS_GAINED:
                hadFocus = true;
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL);
                break;
              case SDL_WINDOWEVENT_FOCUS_LOST:
                hadFocus = false;
                SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);
                break;

              default: break;
            }
            break;

          case SDL_KEYUP:
          case SDL_KEYDOWN:
            engine->ProcessInputEvent(MakeSDLKey(e.key));
            break;
        }
      }
    }

    // Render
    if (hadFocus) {
      // make sure we have the optimal tick rate
      if (std::chrono::duration_cast<std::remove_cv<decltype(OPTIMAL_RENDER_DELAY)>::type>(nowPoint - lastRenderTime) > OPTIMAL_RENDER_DELAY) {
        engine->Draw();
        SDL_UpdateWindowSurface(sdlWindow);
      }
    }

    // Don't steal every cycle if we're not focused
    if (!hadFocus) {
      // Yield our timeslice
      Sleep(0);
    }
  }
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PWSTR pCmdLine, int nCmdShow) {
  AllocConsole();

  {
    FILE *newStdout = nullptr;
    FILE *newStderr = nullptr;
    FILE *newStdin = nullptr;

    (void)::freopen_s(&newStdout, "CONOUT$", "w", (__acrt_iob_func(1)));
    (void)::freopen_s(&newStderr, "CONOUT$", "w", (__acrt_iob_func(2)));
    (void)::freopen_s(&newStdin, "CONIN$", "r", (__acrt_iob_func(0)));

    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();

    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  auto engine = CreateGameEngine();
  if (!engine) {
    printf("Unable to start engine");
    return -1;
  }

  printf("--------------------------------------------------------------------------\n");
  printf("Game: %s\n", engine->Name().data());
  printf("--------------------------------------------------------------------------\n");

  SDL_Window *window = SDL_CreateWindow(
    engine->Name().data(),
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    SDL_WINDOW_ALLOW_HIGHDPI);

  std::unique_ptr<SDLBitmap> screen_bitmap = std::make_unique<SDLBitmap>(SDL_GetWindowSurface(window));
  if (const auto ec = engine->SetOutputScreen(screen_bitmap.get())) {
    printf("Error: %s\n", ec.message().data());
    return -1;
  }

  if (const auto ec = engine->Init()) {
    printf("Error: %s\n", ec.message().data());
    return -1;
  }

  {
    e00::Color colors[] = { { 142, 197, 181 }, { 112, 173, 164 }, { 91, 154, 154 }, { 77, 125, 141 }, { 54, 84, 113 }, { 42, 66, 104 }, { 25, 36, 73 }, { 21, 13, 55 }, { 20, 7, 38 }, { 12, 22, 52 }, { 16, 45, 64 }, { 22, 68, 77 }, { 31, 96, 94 }, { 46, 129, 114 }, { 64, 158, 116 }, { 98, 187, 125 }, { 121, 205, 122 }, { 36, 13, 57 }, { 56, 21, 78 }, { 85, 30, 102 }, { 113, 45, 122 }, { 146, 66, 143 }, { 173, 96, 143 }, { 146, 181, 184 }, { 134, 157, 162 }, { 121, 135, 142 }, { 103, 108, 120 }, { 89, 89, 102 }, { 71, 68, 78 }, { 63, 58, 67 }, { 54, 47, 55 }, { 42, 35, 40 }, { 14, 2, 10 }, { 52, 15, 33 }, { 78, 31, 50 }, { 116, 52, 68 }, { 140, 69, 78 }, { 162, 89, 89 }, { 179, 115, 106 }, { 198, 149, 128 }, { 210, 174, 145 }, { 229, 204, 171 }, { 242, 233, 205 }, { 242, 223, 167 }, { 223, 182, 132 }, { 210, 149, 103 }, { 191, 110, 80 }, { 178, 78, 61 }, { 164, 48, 69 }, { 149, 36, 82 } };

    //_system->SetPalette(colors);
  }

  MainLoop(engine, window);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
