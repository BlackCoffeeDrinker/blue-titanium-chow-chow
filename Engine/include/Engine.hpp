#pragma once

#include <array>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <limits>
#include <memory>
#include <map>
#include <system_error>
#include <vector>
#include <iterator>
#include <type_traits>
#include <string_view>
#include <string>
#include <set>
#include <utility>
#include <list>

#include <Engine/Config.hpp>

#include <Engine/Detail/StringFormat.hpp>
#include <Engine/Detail/TypeId.hpp>
#include <Engine/Detail/Array.hpp>
#include <Engine/Detail/CircularBuffer.hpp>

#include <Engine/Logging/SourceLocation.hpp>
#include <Engine/Logging/Logger.hpp>

#include <Engine/Math.hpp>
#include <Engine/SpacePartition.hpp>

#include <Engine/ComponentRegistry.hpp>

#include <Engine/Action.hpp>
#include <Engine/Binding.hpp>
#include <Engine/InputEvent.hpp>
#include <Engine/Stream.hpp>
#include <Engine/Resource.hpp>
#include <Engine/ResourcePtr.hpp>

#include <Engine/GUI/Menu.hpp>
#include "Engine/Resource/Bitmap.hpp"
#include <Engine/GUI/FontGlyph.hpp>
#include "Engine/Resource/Font.hpp"
#include "Engine/Resource/Sprite.hpp"
#include <Engine/GUI/Window.hpp>

#include "Engine/Resource/Tileset.hpp"
#include "Engine/Resource/Map.hpp"
#include <Engine/Actor.hpp>
#include <Engine/World.hpp>

#include <Engine/Scripting/ScriptEngine.hpp>

#include <Engine/Engine.hpp>

/**
 * The `platform` namespace is implemented by platform code
 */
namespace platform {
/**
 * Opens a stream
 * Implemented by the platform code
 *
 * @param name the resource name to open
 * @return a stream, or nullptr
 */
std::unique_ptr<e00::Stream> OpenStream(const std::string &name);

/**
 * Create a logger sink
 * Implemented by platform code
 *
 * @param name the sink name
 * @return a sink, or nullptr
 */
std::unique_ptr<e00::LoggerSink> CreateSink(const std::string& name);
}


/**
 * To be defined by the Game's core;
 * this is what that the platform specific code should call to create the main engine
 * @param sys the system to use
 * @return
 */
std::unique_ptr<e00::Engine> CreateGameEngine();
