#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/LayerGroup.hpp>
#include <tmxlite/TileLayer.hpp>

#include <array>
#include <string>

class TMXParser {
    public:
        TMXParser() {};
        tmx::Map parse_map(const std::string& path_to_level); 
};
