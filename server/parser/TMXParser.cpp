#include "TMXParser.hpp"
#include "tmxlite/Map.hpp"
#include <stdexcept>

tmx::Map TMXParser::parse_map(const std::string& path_to_level) {
    tmx::Map map;
     
    if(map.load(path_to_level)) {
        return map;
    }

    throw std::runtime_error("Failed to load map");
}
