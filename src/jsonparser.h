#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QByteArray>
#include "jsonobject.h"

class JsonParser
{
    public:
        JsonParser(QByteArray code);
        void parse(JsonObject& object);

    private:
        void parseSub(JsonObject& object);
        void skipUnwanted();
        void parseObject(JsonObject& object);
        void parseArray(JsonObject &object);
        QString parseString();
        int parseNumber();

        // code
        QByteArray code;
        QByteArray::iterator itr;

        // json data
        JsonObject data;
        JsonObject* tail = 0;

        char end = '\0';
};

#endif // JSONPARSER_H
