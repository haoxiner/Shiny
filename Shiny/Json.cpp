#include "Json.h"

int Shiny::Json::JsonValue::AsInt() const
{
    if (type_ == JSON_NUMBER) {
        return static_cast<int>(value_.number);
    }
    return 0;
}

float Shiny::Json::JsonValue::AsFloat() const
{
    if (type_ == JSON_NUMBER) {
        return static_cast<int>(value_.number);
    }
    return 0.0f;
}

Shiny::Float2 Shiny::Json::JsonValue::AsFloat2() const
{
    if (type_ == JSON_ARRAY) {
        return Float2(value_.array[0], value_.array[1]);
    }
    return Float2();
}

Shiny::Float3 Shiny::Json::JsonValue::AsFloat3() const
{
    if (type_ == JSON_ARRAY) {
        return Float3(value_.array[0], value_.array[1], value_.array[2]);
    }
    return Float3();
}

Shiny::Float4 Shiny::Json::JsonValue::AsFloat4() const
{
    if (type_ == JSON_ARRAY) {
        return Float4(value_.array[0], value_.array[1], value_.array[2], value_.array[3]);
    }
    return Float4();
}

Shiny::Matrix4x4 Shiny::Json::JsonValue::AsMatrix4x4() const
{
    if (type_ == JSON_ARRAY) {
        return Matrix4x4(
            value_.array[0], value_.array[1], value_.array[2], value_.array[3],
            value_.array[4], value_.array[5], value_.array[6], value_.array[7],
            value_.array[8], value_.array[9], value_.array[10], value_.array[11],
            value_.array[12], value_.array[13], value_.array[14], value_.array[15]);
    }
    return Matrix4x4();
}

std::string Shiny::Json::JsonValue::AsString() const
{
    switch (type_) {
    case Shiny::Json::JSON_NULL:
        return std::string("null");
        break;
    case Shiny::Json::JSON_BOOL:
        return std::to_string(value_.boolean);
        break;
    case Shiny::Json::JSON_NUMBER:
        return std::to_string(value_.number);
        break;
    case Shiny::Json::JSON_STRING:
        return std::string(value_.str);
        break;
    case Shiny::Json::JSON_OBJECT:
        return std::string("JSON_OBJECT");
        break;
    case Shiny::Json::JSON_ARRAY:
        return std::string("JSON_ARRAY");
        break;
    default:
        break;
    }
    return std::string();
}

const Shiny::Json::JsonArray* Shiny::Json::JsonValue::AsJsonArray() const
{
    if (type_ == JSON_ARRAY) {
        return value_.array;
    }
    return nullptr;
}

const Shiny::Json::JsonObject* Shiny::Json::JsonValue::AsJsonObject() const
{
    if (type_ == JSON_OBJECT) {
        return value_.object;
    }
    return nullptr;
}

Shiny::Json::JsonValue* Shiny::Json::JsonObject::GetValue(std::string& key)
{
    auto valueIter = valueTable_.find(key);
    if (valueIter != valueTable_.end()) {
        return valueIter->second;
    }
    return nullptr;
}

Shiny::Json::JsonValue* Shiny::Json::JsonArray::operator[](int i)
{
    return array_[i];
}

class ParserState
{
public:
    ParserState(const std::string& json): length(json.length())
    {
    }
    void Forward()
    {
        offset++;
    }

    
};

bool Shiny::Json::Parse(JsonObject& jsonObject, const std::string& json)
{
    size_t offset = 0;
    const size_t length = json.length();
    enum ParserState
    {
        READY,
        OBJECT_BEGIN,
        STOP
    };
    ParserState state = READY;
    if (offset < length) {

    }
    while (offset < length && state != STOP) {
        char ch = json[offset];
        switch (ch) {
        case '{':
            state = OBJECT_BEGIN;
            break;
        default:
            break;
        }
    }
    return false;
}
