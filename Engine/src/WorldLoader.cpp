
#include "WorldLoader.hpp"
#include "EngineError.hpp"
#include "nlohmann/json.hpp"

namespace e00::impl {

class WorldLoader::Impl {
  mutable Logger _logger;
  nlohmann::json _world_json;

public:
  Impl() : _logger(platform::CreateSink("WorldLoader")){};
  ~Impl() = default;

  template<typename InputType>
  void parse(InputType &&i) { _world_json = nlohmann::json::parse(i, nullptr, false, true); }

  template<typename KeyType>
  bool contains(KeyType &&key) const {
    if (!_world_json.contains(key)) {
      _logger.Log(source_location::current(), L_ERROR, "World is not valid, missing key {}", std::string_view{ key });
      return false;
    }
    return true;
  }

  template<typename... KeyType>
  bool contains_all(KeyType &&...keys) const {
    return (contains(keys) && ...);
  }

  template<typename KeyType, typename T>
  bool get(KeyType &&key, T &output) const {
    const auto &a = _world_json.find(key);
    if (a != _world_json.end()) {
      output = (*a).template get<T>();
      return true;
    }

    _logger.Log(source_location::current(), L_ERROR, "World is not valid, missing key {}", std::string_view{ key });
    return false;
  }

  [[nodiscard]] auto &world_json() const { return _world_json; }
};

WorldLoader::WorldLoader(const std::string &name, const std::unique_ptr<Stream> &worldStream)
  : _logger(platform::CreateSink("WorldLoader")),
    _impl(new WorldLoader::Impl),
    _name(name),
    _input(worldStream) {
  // TODO: Don't load everything into memory twice
  std::string json_str;
  json_str.resize(_input->stream_size());
  _input->read(_input->stream_size(), json_str.data());

  // Parse the world file & make sure it's valid
  _impl->parse(json_str);
}

WorldLoader::~WorldLoader() {
  delete _impl;
}

bool WorldLoader::is_valid() const {
  // Make sure the JSON wasn't discarded
  if (_impl->world_json().is_discarded()) {
    return false;
  }

  // Make sure we have the minimum required fields
  if (!_impl->contains_all("width", "height", "type", "layers", "tilesets")) {
    return false;
  }


  return true;
}

World WorldLoader::build() const {
  // Create the container
  World tmpWorld(_name);

  int width, height;
  if (!_impl->get("width", width) || !_impl->get("height", height)) {
    // todo
    abort();
  }

  // Read the map
  const auto &layers = _impl->world_json().find("layers");
  if (layers == _impl->world_json().end() || !layers->is_array()) {
    // todo
    abort();
  }

  // Read all the layers
  // TODO: this only reads one for now
  for (const auto &layer : *layers) {
    auto layer_width = layer.find("width")->get<int>();
    auto layer_height = layer.find("height")->get<int>();
    auto layer_x = layer.find("x")->get<int>();
    auto layer_y = layer.find("y")->get<int>();
    auto layer_data = layer.find("data");

    if (layer_width != width || layer_height != height) {
      // todo
      abort();
    }

    if (layer_data == layer.end() || !layer_data->is_array()) {
      // todo
      abort();
    }

    Map tmpMap(layer_width, layer_height);
    if (auto ec = tmpMap.LoadBulk(layer_data->begin(), layer_data->end())) {
      // todo
      abort();
    }

    tmpWorld.SetMap(std::move(tmpMap));
    break;
  }

  // Make the tilemap
  const auto &tilesets = _impl->world_json().find("tilesets");
  if (tilesets == _impl->world_json().end() || !tilesets->is_array()) {
    // todo
    abort();
  }

  for (const auto& tileset : *tilesets) {
    const auto firstgid = tileset.find("firstgid")->get<int>();
    const auto tilewidth = tileset.find("tilewidth")->get<int>();
    const auto tileheight = tileset.find("tileheight")->get<int>();
    const auto spacing = tileset.find("spacing")->get<int>();
    const auto margin = tileset.find("margin")->get<int>();
    const auto tilecount = tileset.find("tilecount")->get<int>();
    const auto image = tileset.find("image")->get<std::string>();
    const auto name = tileset.find("name")->get<std::string>();

    _logger.Log(source_location::current(), L_VERBOSE, "Loading tileset {}", name);

    Tileset tmpSet(tilecount);


  }


  return tmpWorld;
}


}// namespace e00::impl
