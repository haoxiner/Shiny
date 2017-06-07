#pragma once
#include "MathUtil.h"
#include <string>
#include <vector>
#include <map>
namespace Shiny
{
namespace Json
{
enum ValueType
{
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
};
class JsonObject;
class JsonArray;
class JsonValue
{
public:
    int AsInt() const;
    float AsFloat() const;
    Float2 AsFloat2() const;
    Float3 AsFloat3() const;
    Float4 AsFloat4() const;
    Matrix4x4 AsMatrix4x4() const;
    std::string AsString() const;
    const JsonArray* AsJsonArray() const;
    const JsonObject* AsJsonObject() const;
private:
    union Value
    {
        bool boolean;
        float number;
        char* str;
        JsonArray* array;
        JsonObject* object;
    };
    Value value_;
    ValueType type_;
    std::string name_;
};
class JsonObject : public JsonValue
{
    JsonValue* GetValue(std::string& key);
private:
    std::map<std::string, JsonValue*> valueTable_;
};
class JsonArray : public JsonValue
{
public:
    JsonValue* operator[](int i);
private:
    std::vector<JsonValue*> array_;
};
bool Parse(JsonObject& jsonObject, const std::string& json);
}
}