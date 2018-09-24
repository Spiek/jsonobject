#include "jsonparser.h"

JsonParser::JsonParser(QByteArray code) : code(code)
{

}

JsonObject JsonParser::parse()
{
    // init tail
    this->tail = &this->data;

    // loop all chars
    /*for(this->itr = this->code.begin(); this->itr != this->code.end(); this->itr++) {
        // parse data types
        if(*itr == '{') this->parseObject();
        else if(*itr == '[') this->parseArray();
    }*/
    return data;
}

JsonObject JsonParser::parseObject()
{
    itr++;
    QString objectName = this->parseString();
    qDebug() << objectName;
}

JsonObject JsonParser::parseArray()
{

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
