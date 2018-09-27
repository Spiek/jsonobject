#include "jsonparser.h"

JsonParser::JsonParser(QByteArray code) : code(code)
{

}

void JsonParser::parse(JsonObject& object)
{
    qDebug(qPrintable(this->code + "\n"));

    this->itr = this->code.begin();
    this->parseSub(object);
}

void JsonParser::parseSub(JsonObject &object)
{
    this->skipUnwanted();

    // parse object
    if(*itr == '[') this->parseArray(object);
    else if(*itr == '{') this->parseObject(object);
    else if(*itr == '"') object = this->parseString();
    else if(*itr >= 48 && *itr <= 57) object = this->parseNumber();
    else if(*itr       == 't' &&
            *(itr + 1) == 'r' &&
            *(itr + 2) == 'u' &&
            *(itr + 3) == 'e')
    {
        this->itr += 4;
        object = true;
    }
    else if(*itr       == 'f' &&
            *(itr + 1) == 'a' &&
            *(itr + 2) == 'l' &&
            *(itr + 3) == 's' &&
            *(itr + 4) == 'e')
    {
        this->itr += 5;
        object = false;
    } else if(*itr       == 'n' &&
              *(itr + 1) == 'u' &&
              *(itr + 2) == 'l' &&
              *(itr + 3) == 'l')
    {
        this->itr += 4;
    }
}

void JsonParser::parseArray(JsonObject& object)
{
    while(this->itr != this->code.end()) {
        itr++;
        this->skipUnwanted();
        this->parseSub(object());
        this->skipUnwanted();
        if(*itr == ']') {
            itr++;
            break;
        }
    };
}

void JsonParser::parseObject(JsonObject& object)
{
    while(this->itr != this->code.end()) {
        itr++;
        this->skipUnwanted();
        JsonObject& obj = object(this->parseString());
        this->skipUnwanted();
        itr++;
        this->skipUnwanted();
        this->parseSub(obj);
        if(*itr == '}') {
            itr++;
            break;
        }
    };
}

QString JsonParser::parseString()
{
    QString string;
    if(*itr != '"') return string;
    for(itr++; *itr != '"'; itr++) {
        if(*itr == '\\' && *(itr + 1) == '"') itr++;
        string.append(*itr);
    }
    itr++;
    return string;
}

int JsonParser::parseNumber()
{
    QByteArray data;
    for(; this->itr != this->code.end() && *this->itr >= 48 && *this->itr <= 57; this->itr++) data += *this->itr;
    return data.isEmpty() ? 0 : data.toInt();
}

void JsonParser::skipUnwanted()
{
    for(; this->itr != this->code.end(); this->itr++) {
        if(*itr > 32) break;
    }
}

