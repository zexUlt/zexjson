#pragma once

#include "Minimal.hpp"
#include "JsonValue.hpp"

namespace zexjson {

class JsonObject
{
public:
    std::unordered_map<std::string, std::shared_ptr<JsonValue>> Values;

};

} // namespace zexjson