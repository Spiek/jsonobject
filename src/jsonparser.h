#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QByteArray>
#include "jsonobject.h"

class JsonParser
{
    public:
        JsonParser(QByteArray code);
        JsonObject parse();

    private:
        JsonObject parseObject();
        JsonObject parseArray();
        QString parseString();

        // code
        QByteArray code;
        QByteArray::iterator itr;

        // json data
        JsonObject data;
        JsonObject* tail = 0;
};

#endif // JSONPARSER_H
