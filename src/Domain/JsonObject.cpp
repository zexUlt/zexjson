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

void JsonObject::SetNumberField(const std::string& FieldName, double Number)
{
    this->Values[FieldName] = std::make_shared<JsonValueNumber>(Number);
}

std::string JsonObject::GetStringField(const std::string& FieldName) const
{
    return GetField<EJson::None>(FieldName)->AsString();
}

bool JsonObject::TryGetStringField(const std::string& FieldName, std::string& OutString) const
{
    auto Field = TryGetField(FieldName);
    return Field && Field->TryGetString(OutString);
}

bool JsonObject::TryGetStringArrayField(const std::string& FieldName, std::vector<std::string>& OutArray) const
{
    auto Field = TryGetField(FieldName);

    if(!Field){
        return false;
    }

    const std::vector<std::shared_ptr<JsonValue>>* Array;

    if(!Field->TryGetArray(Array)){
        return false;
    }

    for(size_t i = 0; i < Array->size(); ++i){
        std::string Element;

        if(!(*Array)[i]->TryGetString(Element)){
            return false;
        }

        OutArray.emplace_back(Element);
    }

    return true;
}

void JsonObject::SetStringField(const std::string& FieldName, const std::string& StringValue)
{
    this->Values[FieldName] = std::make_shared<JsonValueString>(StringValue);
}

bool JsonObject::GetBoolField(const std::string& FieldName) const
{
    return GetField<EJson::None>(FieldName)->AsBool();
}

bool JsonObject::TryGetBoolField(const std::string& FieldName, bool& OutBool) const
{
    auto Field = TryGetField(FieldName);
    return Field && Field->TryGetBool(OutBool);
}

void JsonObject::SetBoolField(const std::string& FieldName, bool InValue)
{
    this->Values[FieldName] = std::make_shared<JsonValueBoolean>(InValue);
}

const std::vector<std::shared_ptr<JsonValue>>& JsonObject::GetArrayField(const std::string& FieldName) const
{
    return GetField<EJson::Array>(FieldName)->AsArray();
}

bool JsonObject::TryGetArrayField(const std::string& FieldName, const std::vector<std::shared_ptr<JsonValue>>*& OutArray) const
{
    auto Field = TryGetField(FieldName);
    return Field && Field->TryGetArray(OutArray);
}

void JsonObject::SetArrayField(const std::string& FieldName, const std::vector<std::shared_ptr<JsonValue>>& Array)
{
    this->Values[FieldName] = std::make_shared<JsonValueArray>(Array);
}

const std::shared_ptr<JsonObject>& JsonObject::GetObjectField(const std::string& FieldName) const
{
    return GetField<EJson::Object>(FieldName)->AsObject();
}

bool JsonObject::TryGetObjectField(const std::string& FieldName, const std::shared_ptr<JsonObject>*& OutObject) const
{
    auto Field = TryGetField(FieldName);
    return Field && Field->TryGetObject(OutObject);
}

void JsonObject::SetObjectField(const std::string& FieldName, const std::shared_ptr<JsonObject>& JsonObject)
{
    if(JsonObject){
        this->Values[FieldName] = std::make_shared<JsonValueObject>(JsonObject.get());
    }else{
        this->Values[FieldName] = std::make_shared<JsonValueNull>();
    }
}