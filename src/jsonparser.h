#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QByteArray>
#include "jsonobject.h"
#include <QElapsedTimer>

class JsonParser
{
    public:
        static void parse(const QByteArray &code, JsonObject& object);

    private:
        JsonParser(const QByteArray &code);

        // parsers
        void parseSub(JsonObject& object);
        void parseObject(JsonObject& object);
        void parseArray(JsonObject &object);
        QString parseString();
        int parseNumber();

        // helpers
        inline void skipUnwanted() { for(; this->itr != this->code.end() && *itr <= 32; this->itr++); }
        bool nextChars(const char* data, bool skipUnwanted = true, bool remove = true);

        // code
        const QByteArray& code;
        QByteArray::const_iterator itr;
};

#endif // JSONPARSER_H
