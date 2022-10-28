#include "Domain/JsonValue.hpp"

#include <limits>
#include <cmath>

using namespace zexjson;

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

double JsonValue::AsNumber() const
{
    double Number{0.0};

    if(!TryGetNumber(Number)){
        // TODO: Error message here
    }

    return Number;
}

std::string JsonValue::AsString() const
{
    std::string String;

    if(!TryGetString(String)){
        // TODO: Error message here
    }

    return String;
}

bool JsonValue::AsBool() const
{
    bool Bool{false};

    if(!TryGetBool(Bool)){
        // TODO: Error message here
    }

    return Bool;
}

const std::vector<std::shared_ptr<JsonValue>>& JsonValue::AsArray() const
{
    const std::vector<std::shared_ptr<JsonValue>>* Array{nullptr};

    if(!TryGetArray(Array)){
        static const std::vector<std::shared_ptr<JsonValue>> EmptyArray;
        Array = &EmptyArray;
        // TODO: Error message here
    }

    return *Array;
}

const std::shared_ptr<JsonObject>& JsonValue::AsObject() const
{
    const std::shared_ptr<JsonObject>* Object{nullptr};

    if(!TryGetObject(Object)){
        static const std::shared_ptr<JsonObject> EmptyObject = std::make_shared<JsonObject>();
        Object = &EmptyObject;
        // TODO: Error message here
    }

    return *Object;
}

// =====================

template<typename T>
bool TryConvertNumber(const JsonValue& InValue, T& OutNumber)
{
    double Double;

    if(InValue.TryGetNumber(Double) && (Double >= std::numeric_limits<T>::min()) && (Double <= static_cast<double>(std::numeric_limits<T>::max()))){
        OutNumber = static_cast<T>(std::round(Double));

        return true;
    }

    return false;
}

// Special handling for std::int64_t/std::uint64, due to overflow in the numeric limits
// 2^63 - 1 and 2^64 - 1 cannot be exactly represented as a double
// So std::numeric_limits<>::max() gets rounded up to exactly 2^63 or 2^64 by the compilers implicit cast to double
template<>
bool TryConvertNumber<std::uint64_t>(const JsonValue& InValue, std::uint64_t& OutNumber)
{
    double Double;
    const double _2_to_64 = 18446744073709551616.0;

    if(InValue.TryGetNumber(Double) && Double >= 0.0 && Double < _2_to_64){
        OutNumber = static_cast<std::uint64_t>(std::round(Double));

        return true;
    }

    return false;
}

template<>
bool TryConvertNumber<std::int64_t>(const JsonValue& InValue, std::int64_t& OutNumber)
{
    double Double;
    const double _2_to_63 = 9223372036854775808.0;

    if(InValue.TryGetNumber(Double) && Double >= -_2_to_63 && Double <= _2_to_63){
        OutNumber = static_cast<std::int64_t>(std::round(Double));

        return true;
    }

    return false;
}

// =====================

bool JsonValue::TryGetNumber(float& OutNumber) const
{
    double Double;

    if(TryGetNumber(Double)){
        OutNumber = static_cast<float>(Double);

        return true;
    }

    return false;
}

bool JsonValue::TryGetNumber(std::int8_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::int16_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::int32_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::int64_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::uint8_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::uint16_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::uint32_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}

bool JsonValue::TryGetNumber(std::uint64_t& OutNumber) const
{
    return TryConvertNumber(*this, OutNumber);
}


// static
bool JsonValue::CompareEqual(const JsonValue& Lhs, const JsonValue& Rhs)
{
    if(Lhs.Type != Rhs.Type){
        return false;
    }

    switch (Lhs.Type)
    {
    case EJson::None:
    case EJson::Null:
        return true;

    case EJson::String:
        return Lhs.AsString() == Rhs.AsString();

    case EJson::Number:
        return Lhs.AsNumber() == Rhs.AsNumber();

    case EJson::Boolean:
        return Lhs.AsBool() == Rhs.AsBool();

    case EJson::Array:
    {
        const auto& LhsArray = Lhs.AsArray();
        const auto& RhsArray = Rhs.AsArray();

        if(LhsArray.size() != RhsArray.size()){
            return false;
        } 

        for(std::size_t i{0}; i < LhsArray.size(); ++i){
            if(!CompareEqual(*LhsArray[i], *RhsArray[i])){
                return false;
            }
        }

        return true;
    }

    case EJson::Object:
    {
        const auto& LhsObject = Lhs.AsObject();
        const auto& RhsObject = Rhs.AsObject();

        if(bool(LhsObject) != bool(RhsObject)){
            return false;
        }

        if(LhsObject){
            // if(LhsObject->Values)
        }
    }
    }
}

void JsonValue::ErrorMessage(std::string_view InType) const
{
    // Log here
}