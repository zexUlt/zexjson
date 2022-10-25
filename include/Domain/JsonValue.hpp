#pragma once

#include "Minimal.hpp"
#include "Serialization/JsonTypes.hpp"

namespace zexjson{

class JsonObject;

class JsonValue
{
public:

    /** Returns this value as a double, returning zero if this is not an Json Number */
    double AsNumber() const;

    /** Returns this value as a string, returning empty string if not possible */
    std::string AsString() const;

    /** Returns this value as a bool, returning false if not possible */
    bool AsBool() const;

    /** Returns this value as an array, returning an empty array reference if not possible */
    const std::vector<std::shared_ptr<JsonValue>>& AsArray() const;

    /** Returns this value as an object, throwing an error if this is not an Json Object */
    virtual const std::shared_ptr<JsonObject>& AsObject() const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(double& OutNumber) const { return false; }

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(float& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::int8_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::int16_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::int32_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::int64_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::uint8_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::uint16_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::uint32_t& OutNumber) const;

    /** Tries to convert this value to a number, returning false if not possible */
    virtual bool TryGetNumber(std::uint64_t& OutNumber) const;

    /** Tries to convert this value to a string, returning false if not possible */
    virtual bool TryGetString(std::string& OutString) const { return false; }

    /** Tries to convert this value to a bool, returning false if not possible */
    virtual bool TryGetBool(bool& OutBool) const { return false; }
    
    /** Tries to convert this value to an array, returning false if not possible */
    virtual bool TryGetArray(const std::vector<std::shared_ptr<JsonValue>>*& OutArray) const { return false; }    

    /** Tries to convert this value to an object, returning false if not possible */
    virtual bool TryGetObject(const std::shared_ptr<JsonObject>*& OutObject) const { return false; }

    /** Returns true if this value is a 'null '*/
    bool IsNull() const;

    /** Get a field of the same type as the argument */
    void AsArgumentType(double& Value);
    void AsArgumentType(std::string& Value);
    void AsArgumentType(bool& Value);
    void AsArgumentType(std::vector<std::shared_ptr<JsonValue>>& Value);
    void AsArgumentType(std::shared_ptr<JsonObject>& Value);

    EJson Type;

    static bool CompareEqual(const JsonValue& Lhs, const JsonValue& Rhs);

protected:
    JsonValue();
    virtual ~JsonValue();

    virtual std::string GetType() const = 0;

    void ErrorMessage(std::string_view InType) const;
};

inline bool operator==(const JsonValue& Lhs, const JsonValue& Rhs);
inline bool operator!=(const JsonValue& Lhs, const JsonValue& Rhs);


/** A Json String Value. */
class JsonValueString : public JsonValue
{
public:
    JsonValueString(const string_view InString);

    virtual bool TryGetString(std::string& OutString) const override;
    virtual bool TryGetNumber(double& OutNumber) const override;
    virtual bool TryGetNumber(std::int32_t& OutNumber) const override;
    virtual bool TryGetNumber(std::uint32_t& OutNumber) const override;
    virtual bool TryGetNumber(std::int64_t& OutNumber) const override;
    virtual bool TryGetNumber(std::uint64_t& OutNumber) const override;
    virtual bool TryGetBool(bool& OutBool) const override;

    // Way to check if string value is empty without copying the string
    bool IsEmpty() const;

protected:
    std::string Value;

    virtual std::string GetType() const override;
}


/** A Json Number Value. */
class JsonValueNumber : public JsonValue
{
public:
    JsonValueNumber(double InNumber);

    virtual bool TryGetNumber(double& OutNumber) const override;
    virtual bool TryGetBool(bool& OutBool) const override;
    virtual bool TryGetString(std::string& OutString) const override;

protected:
    double Value;

    virtual std::string GetType() const override;
}


/** A Json Boolean Value. */
class JsonValueBoolean : public JsonValue
{
public:
    JsonValueBoolean(bool InBool);

    virtual bool TryGetNumber(double& OutNumber) const override;
    virtual bool TryGetBool(bool& OutBool) const override;
    virtual bool TryGetString(std::string& OutString) const override;

protected:
    bool Value;

    virtual std::string GetType() const override;
}


/** A Json Array Value. */
class JsonValueArray : public JsonValue
{
public: 
    JsonValueArray(const std::vector<std::shared_ptr<JsonValue>>& InArray);

    virtual bool TryGetArray(const std::vector<std::shared_ptr<JsonValue>>*& OutArray) const override;

protected:
    std::vector<std::shared_ptr<JsonValue>> Value;

    virtual std::string GetType() const override;
}


/** A Json Object Value. */
class JsonValueObject : public JsonValue
{
public: 
    JsonValueObject(std::shared_ptr<JsonObject> InObject);

    virtual bool TryGetObject(const std::shared_ptr<JsonObject>*& OutObject) const override;

protected:
    std::shared_ptr<JsonObject> Value;

    virtual std::string GetType() const override;
}


/** A Json Null Value. */
class JsonValueNull : public JsonValue
{
public:
    JsonValueNull();

protected:
    virtual std::string GetType() const override;
}
} // namespace zexjson