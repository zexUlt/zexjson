#include "Domain/JsonObject.hpp"

using namespace zexjson;

void JsonObject::SetField(const std::string& FieldName, const std::shared_ptr<JsonValue>& Value)
{
    this->Values[FieldName] = Value;
}

void JsonObject::RemoveField(const std::string& FieldName)
{
    this->Values.erase(FieldName);
}

double JsonObject::GetNumberField(const std::string& FieldName) const
{
    return GetField<EJson::None>(FieldName)->AsNumber();
}

bool JsonObject::TryGetNumberField(const std::string& FieldName, double& OutNumber) const
{
    auto Field = TryGetField(FieldName);
    return Field && Field->TryGetNumber(OutNumber);
}