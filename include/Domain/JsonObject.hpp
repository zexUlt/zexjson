#pragma once

#include "Minimal.hpp"
#include "JsonValue.hpp"

namespace zexjson {

class JsonObject
{
public:
    std::unordered_map<std::string, std::shared_ptr<JsonValue>> Values;

    template<EJson JsonType>
    std::shared_ptr<JsonValue> GetField(const std::string& FieldName) const
    {
        const auto FieldIt = Values.find(FieldName);
        if(FieldIt != Values.end()){
            const auto* [Name, Value] = *FieldIt;
            if(Value){
                if(JsonType == EJson::None || (*Value)->Type == JsonType){
                    return (*Value);
                }else{
                    // LOG: Field of a wrong type
                }
            }
        }else{
            // LOG: Field not found
        }

        return std::make_shared<JsonValueNull>();
    }

    /**
     * Attempts to get the field with the specified name.
     * 
     * @param FieldName The name of the field to get.
     * @return A pointer to the field, or @c nullptr if the field doesn't exist.
    */
   std::shared_ptr<JsonValue> TryGetField(const std::string& FieldName) const
   {
        const auto FieldIt = Values.find(FieldName);
        return (FieldIt != Values.end() ? FieldIt->second : std::shared_ptr<JsonValue>());
   }

   /**
    * Checks whether a field with the specified name exists in the object
    * 
    * @param FieldName The name of the field to check.
    * @return @c true if the field exists, @c false otherwise.
   */
    bool HasField(const std::string& FieldName) const
    {
        const auto FieldIt = Values.find(FieldName);
        
        return FieldIt != Values.end() && FieldIt->second;
    }

    /**
     * Checks whether a field with the specified name and type exists in the object.
     * 
     * @param JsonType The type of the field to check
     * @param FieldName The name of the field to get.
     * @return A pointer to the field, or @c nullptr if the field doesn't exist.
    */
    template<EJson JsonType>
    bool HasTypedField(const std::string& FieldName) const
    {
        const auto FieldIt = Values.find(FieldName);
        
        return (FieldIt != Values.end() && FieldIt->second && 
                FieldIt->second->Type == JsonType);
    }

    /**
     * Sets the value of the field with the specified name.
     * 
     * @param FieldName The name of the field to set.
     * @param Value The value to set.
    */
    void SetField(const std::string& FieldName, const std::shared_ptr<JsonValue>& Value);

    /**
     * Removes the field with the specified name
     * 
     * @param FieldName The name of the field to set. 
    */
    void RemoveField(const std::string& FieldName);

    /**
     * Gets the field with the specified name as a number.
     * 
     * Ensures that the field is present and is of type @c JsonNumber.
     * 
     * @param FieldName The name of the field to get.
     * @return The field's value as a number.
    */
    double GetNumberField(const std::string& FieldName) const;

    /**
     * Gets a numeric field and casts to an std::int32_t
    */
    inline std::int32_t GetIntegerField(const std::string& FieldName) const
    {
        return (std::int32_t)GetNumberField(FieldName);
    }

    /** Get the field named FieldName as a number. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetNumberField(const std::string& FieldName, double& OutNumber) const;

	/** Get the field named FieldName as a number, and makes sure it's within int32 range. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetNumberField(const std::string& FieldName, std::int32_t& OutNumber) const;

	/** Get the field named FieldName as a number, and makes sure it's within uint32 range. Returns false if it doesn't exist or cannot be converted.  */
	bool TryGetNumberField(const std::string& FieldName, std::uint32_t& OutNumber) const;

	/** Get the field named FieldName as a number. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetNumberField(const std::string& FieldName, std::int64_t& OutNumber) const;

	/** Add a field named FieldName with Number as value */
	void SetNumberField( const std::string& FieldName, double Number );

	/** Get the field named FieldName as a string. */
	std::string GetStringField(const std::string& FieldName) const;

	/** Get the field named FieldName as a string. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetStringField(const std::string& FieldName, std::string& OutString) const;

	/** Get the field named FieldName as an array of strings. Returns false if it doesn't exist or any member cannot be converted. */
	bool TryGetStringArrayField(const std::string& FieldName, std::vector<std::string>& OutArray) const;

    /** Add a field named @c FieldName with value of @c StringValue */
    void SetStringField(const std::string& FieldName, const std::string& StringValue);

    /**
	 * Gets the field with the specified name as a boolean.
	 *
	 * Ensures that the field is present and is of type Json number.
	 *
	 * @param FieldName The name of the field to get.
	 * @return The field's value as a boolean.
	 */
	bool GetBoolField(const std::string& FieldName) const;

	/** Get the field named FieldName as a string. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetBoolField(const std::string& FieldName, bool& OutBool) const;

	/** Set a boolean field named FieldName and value of InValue */
	void SetBoolField(const std::string& FieldName, bool InValue);

	/** Get the field named FieldName as an array. */
	const std::vector<std::shared_ptr<JsonValue>>& GetArrayField(const std::string& FieldName) const;

	/** Try to get the field named FieldName as an array, or return false if it's another type */
	bool TryGetArrayField(const std::string& FieldName, const std::vector<std::shared_ptr<JsonValue>>*& OutArray) const;

	/** Set an array field named FieldName and value of Array */
	void SetArrayField(const std::string& FieldName, const std::vector<std::shared_ptr<JsonValue>>& Array);

	/**
	 * Gets the field with the specified name as a Json object.
	 *
	 * Ensures that the field is present and is of type Json object.
	 *
	 * @param FieldName The name of the field to get.
	 * @return The field's value as a Json object.
	 */
	const std::shared_ptr<JsonObject>& GetObjectField(const std::string& FieldName) const;

	/** Try to get the field named FieldName as an object, or return false if it's another type */
	bool TryGetObjectField(const std::string& FieldName, const std::shared_ptr<JsonObject>*& OutObject) const;

	/** Set an ObjectField named FieldName and value of JsonObject */
	void SetObjectField(const std::string& FieldName, const std::shared_ptr<JsonObject>& JsonObject);
};

} // namespace zexjson