#pragma once
#include "MathUtil.h"
#include <string>
#include <vector>
#include <map>
#include <utility>
namespace Shiny
{
namespace Json
{
enum ValueType
{
    JSON_NULL = -5,
    JSON_BOOL = -4,
    JSON_NUMBER = -3,
    JSON_STRING = -2,
    JSON_OBJECT = -1,
    JSON_ARRAY = 0
};
class JsonObject;
class JsonValue
{
public:
    JsonValue();
    JsonValue(float number);
    JsonValue(bool boolean);
    JsonValue(const char* str);
    JsonValue(const JsonObject* object);
    void Destroy();
    int AsInt() const;
    float AsFloat() const;
    Float2 AsFloat2() const;
    Float3 AsFloat3() const;
    Float4 AsFloat4() const;
    Matrix4x4 AsMatrix4x4() const;
    std::string AsString() const;
    const JsonValue* AsJsonArray() const;
    const JsonObject* AsJsonObject() const;
private:
    union Value
    {
        Value(bool value) : boolean(value) {}
        Value(float value) : number(value) {}
        Value(const char* value) : str(value) {}
        Value(const JsonObject* value) : object(value) {}
        Value(const JsonValue* value) : array(value) {}
        const bool boolean;
        const float number;
        const char* str;
        const JsonObject* object;
        const JsonValue* array;
    };
    const Value value_ = { false };
    const ValueType type_ = JSON_NULL;
    static const JsonValue NULL_OBJECT;
};
class Parser
{
public:
    Parser(JsonObject* jsonObject, const char* json, const size_t length);
    bool HasError();
private:
    bool Forward();
    void Forward(size_t count);
    bool SkipSpaces();
    bool SkipComments();
    void SkipSpacesAndComments();

    bool ParseObject(JsonObject* object);

    std::pair<size_t, size_t> FindStringRange();
    std::string ParseString();
    float ParseNumber();

    const char* json_ = nullptr;
    const size_t length_ = 0;
    size_t offset_ = 0;
    JsonObject* jsonObject_;
    char ch_ = 0;
    bool hasError_ = false;
    bool finish_ = false;
    size_t row_ = 1;
    size_t column_ = 1;
};
class JsonObject
{
    friend class Parser;
public:
    ~JsonObject();
    JsonValue& GetValue(const std::string& key);
private:
    std::map<std::string, JsonValue> valueTable_;
};
//class JsonArray
//{
//public:
//    JsonValue& operator[](int i);
//private:
//    std::vector<JsonValue> array_;
//};
bool Parse(JsonObject& jsonObject, const char* json, const size_t length);
}
}