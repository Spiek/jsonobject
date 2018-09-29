#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QByteArray>
#include "jsonobject.h"
#include <QElapsedTimer>

struct JsonParseResult
{
    // operators
    operator bool() { return this->offset == -1; }

    // error data
    int offset = -1;
    QString expected;
    QString type;
    QByteArray parsed;
    QChar unexpexted;

    // error message builder
    QString error() {
        if(*this) return QString();
        return QString("Unexpected %1 '%3' at position %4, expected '%2'").arg(type, expected).arg(unexpexted.isNull() ? "" : QString(unexpexted)).arg(offset);
    }
};

class JsonParser
{
    public:
        static JsonParseResult parse(const QByteArray &code, JsonObject& object);

    private:
        JsonParser(const QByteArray &code);

        // parsers
        bool parseSub(JsonObject& object);
        void parseObject(JsonObject& object);
        void parseArray(JsonObject &object);
        QString parseString();
        int parseNumber();

        // helpers
        inline void skipUnwanted() { for(; this->itr != this->code.end() && *itr <= 32; this->itr++); }
        bool nextChars(const char* data, bool error, bool skipUnwanted = true, bool remove = true);
        void handleError(QString expected = QString(), size_t offset = 0, QString type = "char");

        // code
        const QByteArray& code;
        QByteArray::const_iterator itr;
        JsonParseResult result;
};

#endif // JSONPARSER_H
