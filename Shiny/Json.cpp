#include "Json.h"
#include <cctype>

const Shiny::Json::JsonValue Shiny::Json::JsonValue::NULL_OBJECT;

Shiny::Json::JsonValue::JsonValue() : type_(JSON_NULL), value_({ false })
{
}

Shiny::Json::JsonValue::JsonValue(float number) : type_(JSON_NUMBER), value_({ number })
{
}

Shiny::Json::JsonValue::JsonValue(bool boolean) : type_(JSON_BOOL), value_({ boolean })
{
}

Shiny::Json::JsonValue::JsonValue(const char* str) : type_(JSON_STRING), value_({ str })
{
}

Shiny::Json::JsonValue::JsonValue(const JsonObject* object) : type_(JSON_OBJECT), value_({ object })
{
}

void Shiny::Json::JsonValue::Destroy()
{
    if (type_ == JSON_STRING) {
        delete[] value_.str;
    } else if (type_ == JSON_OBJECT) {
        delete value_.object;
    } else if (type_ >= JSON_ARRAY) {
        delete value_.array;
    }
}

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
    if (type_ == JSON_ARRAY + 2) {
        return Float2(value_.array[0].AsFloat(), value_.array[1].AsFloat());
    }
    return Float2();
}

Shiny::Float3 Shiny::Json::JsonValue::AsFloat3() const
{
    if (type_ == JSON_ARRAY + 3) {
        return Float3(value_.array[0].AsFloat(), value_.array[1].AsFloat(), value_.array[2].AsFloat());
    }
    return Float3();
}

Shiny::Float4 Shiny::Json::JsonValue::AsFloat4() const
{
    if (type_ == JSON_ARRAY + 4) {
        return Float4(value_.array[0].AsFloat(), value_.array[1].AsFloat(), value_.array[2].AsFloat(), value_.array[3].AsFloat());
    }
    return Float4();
}

Shiny::Matrix4x4 Shiny::Json::JsonValue::AsMatrix4x4() const
{
    if (type_ == JSON_ARRAY + 16) {
        return Matrix4x4(
            value_.array[0].AsFloat(), value_.array[1].AsFloat(), value_.array[2].AsFloat(), value_.array[3].AsFloat(),
            value_.array[4].AsFloat(), value_.array[5].AsFloat(), value_.array[6].AsFloat(), value_.array[7].AsFloat(),
            value_.array[8].AsFloat(), value_.array[9].AsFloat(), value_.array[10].AsFloat(), value_.array[11].AsFloat(),
            value_.array[12].AsFloat(), value_.array[13].AsFloat(), value_.array[14].AsFloat(), value_.array[15].AsFloat());
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
    default:
        return std::string("JSON_ARRAY SIZE = " + std::to_string(type_));
    }
    return std::string();
}

const Shiny::Json::JsonValue* Shiny::Json::JsonValue::AsJsonArray() const
{
    if (type_ >= JSON_ARRAY) {
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

Shiny::Json::JsonObject::~JsonObject()
{
    for (auto&& keyValue : valueTable_) {
        keyValue.second.Destroy();
    }
}

Shiny::Json::JsonValue& Shiny::Json::JsonObject::GetValue(const std::string& key)
{
    auto valueIter = valueTable_.find(key);
    if (valueIter != valueTable_.end()) {
        return valueIter->second;
    }
    return JsonValue();
}

bool Shiny::Json::Parse(JsonObject& jsonObject, const char* json, const size_t length)
{
    return false;
}

Shiny::Json::Parser::Parser(JsonObject* jsonObject, const char* json, const size_t length)
    : json_(json), length_(length), jsonObject_(jsonObject)
{
    ch_ = json_[0];
    SkipSpacesAndComments();
    if (ch_ != '{') {
        hasError_ = true;
    }
    ParseObject(jsonObject);
}

bool Shiny::Json::Parser::HasError()
{
    return hasError_;
}

bool Shiny::Json::Parser::Forward()
{
    if (finish_ || hasError_) {
        return false;
    }
    if (ch_ == '\n') {
        column_ = 1;
        row_++;
    } else {
        column_++;
    }
    offset_++;
    if (offset_ >= length_) {
        return false;
    }
    ch_ = json_[offset_];
    if (ch_ == 0) {
        hasError_ = true;
        return false;
    }
    return true;
}

void Shiny::Json::Parser::Forward(size_t count)
{
    for (size_t i = 0; i < count; i++) {
        Forward();
    }
}

bool Shiny::Json::Parser::SkipSpaces()
{
    bool found = false;
    if (std::isspace(ch_)) {
        found = true;
        while (Forward() && std::isspace(ch_)) {}
    }
    return found;
}

bool Shiny::Json::Parser::SkipComments()
{
    bool found = false;
    if (ch_ == '/') {
        if (Forward() && ch_ == '/') {
            found = true;
            int nextRow = row_ + 1;
            while (Forward() && row_ < nextRow) {}
        }
    }
    return found;
}

void Shiny::Json::Parser::SkipSpacesAndComments()
{
    while (SkipSpaces() || SkipComments()) {
        // break until there are no spaces and comments
    }
}

bool Shiny::Json::Parser::ParseObject(JsonObject* object)
{
    while (Forward() && ch_ != '}') {
        SkipSpacesAndComments();
        if (ch_ == '"') {
            auto key = ParseString();
            if (hasError_) {
                break;
            }
            if (jsonObject_->valueTable_.find(key) != jsonObject_->valueTable_.end()) {
                hasError_ = true;
                break;
            }
            SkipSpacesAndComments();
            if (ch_ == ':') {
                Forward();
                SkipSpacesAndComments();
                auto str = ParseString();
                if (hasError_) {
                    break;
                } else {
                    char* value = new char[str.length() + 1];
                    str.copy(value, str.length());
                    value[str.length()] = '\0';
                    jsonObject_->valueTable_.emplace(std::make_pair(key, JsonValue(value)));
                }
            }
        }
    }
    return false;
}

std::pair<size_t, size_t> Shiny::Json::Parser::FindStringRange()
{
    SkipSpacesAndComments();
    if (ch_ != '"') {
        hasError_ = true;
        return std::pair<size_t, size_t>(0, 0);
    }
    auto begin = offset_;
    auto end = begin;
    while (end < length_ && json_[end] != '"') {
        end++;
    }
    if (end < length_ && json_[end] == '"') {
        end++;
        return std::pair<size_t, size_t>(begin, end);
    }
    hasError_ = true;
    return std::pair<size_t, size_t>(0, 0);
}

std::string Shiny::Json::Parser::ParseString()
{
    if (ch_ != '"') {
        hasError_ = true;
        return "";
    }
    auto begin = offset_;
    auto end = begin + 1;
    while (end < length_ && json_[end] != '"') {
        end++;
    }
    if (end < length_ && json_[end] == '"') {
        end++;
        Forward(end - begin);
        if (begin + 2 == end) {
            return "";
        }
        return std::string(json_ + begin + 1, json_ + end - 1);
    }
    hasError_ = true;
    return "";
}

float Shiny::Json::Parser::ParseNumber()
{
    return 0.0f;
}
