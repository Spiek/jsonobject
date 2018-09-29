#include "jsonparser.h"

JsonParser::JsonParser(const QByteArray &code) : code(code), itr(code.cbegin()) { }

void JsonParser::parse(const QByteArray &code, JsonObject& object)
{
    JsonParser(code).parseSub(object);
}

void JsonParser::parseSub(JsonObject &object)
{
    this->skipUnwanted();

    // parse object
    if(this->nextChars("[", false)) this->parseArray(object);
    else if(this->nextChars("{", false)) this->parseObject(object);
    else if(this->nextChars("\"", false, false)) object = this->parseString();
    else if(*itr >= 48 && *itr <= 57) object = this->parseNumber();
    else if(this->nextChars("true", false)) object = true;
    else if(this->nextChars("false", false)) object = false;
    else if(this->nextChars("null", false)) {}

    this->skipUnwanted();
}

bool JsonParser::nextChars(const char *data, bool skipUnwanted, bool remove)
{
    if(skipUnwanted) this->skipUnwanted();
    size_t len = strlen(data);
    size_t i = 0;
    for(; i < len && (this->itr + i) != this->code.end() && *(this->itr + i) == *(data + i); i++);
    if(i != len) return false;
    if(remove) this->itr += len;
    if(skipUnwanted) this->skipUnwanted();
    return true;
}

void JsonParser::parseArray(JsonObject& object)
{
    for(;this->itr != this->code.end(); itr++) {
        this->parseSub(object());
        if(*itr == ']') break;
    };
    this->nextChars("]");
}

void JsonParser::parseObject(JsonObject& object)
{
    for(;this->itr != this->code.end(); itr++) {
        JsonObject& obj = object(this->parseString());
        this->nextChars(":");
        this->parseSub(obj);
        if(*itr == '}') break;
    };
    this->nextChars("}");
}

QString JsonParser::parseString()
{
    QString string = "";
    this->nextChars("\"");
    for(;this->itr != this->code.end(); itr++) {
        // handle escape sequences
        if(*itr == '\\') {
            if(++this->itr == this->code.end()) break;
            if(*itr == '"') string.append('"');
            else if(*itr == '\\') string.append('\\');
            else if(*itr == 'n') string.append('\n');
            else if(*itr == 'r') string.append('\r');
            else if(*itr == 't') string.append('\t');
            else if(*itr == '\'') string.append('\'');
            else if(*itr == 'b') string.append('\b');
            else if(*itr == 'f') string.append('\f');
            continue;
        }
        if(*itr == '"') break;
        string.append(*itr);
    }
    this->nextChars("\"");
    return string;
}

int JsonParser::parseNumber()
{
    QByteArray data;
    for(; this->itr != this->code.end() && *this->itr >= 48 && *this->itr <= 57; this->itr++) data += *this->itr;
    return data.isEmpty() ? 0 : data.toInt();
}
