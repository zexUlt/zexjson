#include "Domain/JsonValue.hpp"

bool JsonValue::IsNull() const
{
    return Type == EJson::Null || Type == EJson::None;
}

void JsonValue::AsArgumentType(double& Value)
{
    Value = AsNumber();
}

void JsonValue::AsArgumentType(std::string& Value)
{
    Value = AsString();
}

void JsonValue::AsArgumentType(bool& Value)
{
    Value = AsBool();
}

void JsonValue::AsArgumentType(std::vector<std::shared_ptr<JsonValue>>& Value)
{
    Value = AsArray();
}

void JsonValue::AsArgumentType(std::shared_ptr<JsonObject>& Value)
{
    Value = AsObject();
}

inline bool operator==(const JsonValue& Lhs, const JsonValue& Rhs)
{
    return JsonValue::CompareEqual(Lhs, Rhs);
}

inline bool operator!=(const JsonValue& Lhs, const JsonValue& Rhs)
{
    return !JsonValue::CompareEqual(Lhs, Rhs);
}