#include "jsonparser.h"

JsonParser::JsonParser(const QByteArray &code) : code(code), itr(code.cbegin()) { }

JsonParseResult JsonParser::parse(const QByteArray &code, JsonObject& object)
{
    JsonParser parser(code);
    parser.parseSub(object);
    return parser.result;
}

bool JsonParser::parseSub(JsonObject &object)
{
    this->skipUnwanted();

    // parse object
    if(this->nextChars("[", false, false)) this->parseArray(object);
    else if(this->nextChars("{", false, false)) this->parseObject(object);
    else if(this->nextChars("\"", false, false, false)) object = this->parseString();
    else if(*itr >= 48 && *itr <= 57) object = this->parseNumber();
    else if(this->nextChars("true", false, false)) object = true;
    else if(this->nextChars("false", false, false)) object = false;
    else if(this->nextChars("null", false, false)) {}
    else this->handleError("[,{,\",true,false,null or a number");

    // exit on error
    if(!this->result) return false;

    this->skipUnwanted();
    return true;
}

void JsonParser::skipUnwanted()
{
    for(; this->itr != this->code.end(); this->itr++) {
        if(*itr <= 32) continue;

        // handle comments
        if(*itr == '/') {
            if((this->itr + 1) == this->code.cend()) continue;

            // ignore single line comment (from: // to line end)
            if(*(itr + 1) == '/') {
                this->itr += 2;
                for(;;this->itr++) {
                    if((this->itr + 1) == this->code.end()) break;
                    if(*itr == 10) break;
                }
                continue;
            }

            // ignore multi line comment (from: /* to */)
            if(*(itr + 1) == '*') {
                this->itr += 2;
                for(; this->itr != this->code.end(); this->itr++) {
                    if((this->itr + 1) == this->code.end()) break;
                    if(*itr == '*' && *(itr + 1) == '/') {
                        this->itr++;
                        break;
                    }
                }
                continue;
            }
        }
        break;
    }
}

bool JsonParser::nextChars(const char *data, bool error, bool skipUnwanted, bool remove)
{
    if(!this->result) return false;
    if(skipUnwanted) this->skipUnwanted();
    size_t len = strlen(data);
    size_t i = 0;
    for(; i < len && (this->itr + i) != this->code.end() && *(this->itr + i) == *(data + i); i++);
    if(i != len) {
        if(error) this->handleError((data + i), i);
        return false;
    }
	if(remove) this->itr += len;
    if(skipUnwanted) this->skipUnwanted();
    return true;
}

void JsonParser::handleError(QString expected, size_t offset, QString type)
{
    this->result.offset = this->itr - this->code.cbegin() + static_cast<int>(offset);
    this->result.expected = expected;
    this->result.type = type;
    this->result.parsed = QByteArray(this->code.cbegin(),
                                        (this->itr == this->code.cend() ? this->itr : this->itr + 1) - this->code.cbegin());
    this->result.unexpexted = *(this->itr + offset);
}

void JsonParser::parseArray(JsonObject& object)
{
    while(true) {
        if(!this->parseSub(object())) return;
        if(*itr == ']') break;
        if(!this->nextChars(",", true)) return;
    };
    this->nextChars("]", true);
}

void JsonParser::parseObject(JsonObject& object)
{
    while(true) {
        JsonObject& obj = object(this->parseString());
        if(!this->nextChars(":", true)) return;
        if(!this->parseSub(obj)) return;
        if(*itr == '}') break;
        if(!this->nextChars(",", true)) return;
    };
    this->nextChars("}", true);
}

QString JsonParser::parseString()
{
    QString string = "";
    if(!this->nextChars("\"", true)) return string;
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
            else {
                this->handleError("\",\\,n,r,t,',b,f", 0, "escape sequence");
                return "";
            }
            continue;
        }
        if(*itr == '"') break;
        string.append(*itr);

        // if we reach end of string but are not finish, raise error
        if(this->itr == this->code.cend() - 1) {
            this->handleError("\"", 0, "end of file");
            return "";
        }
    }
    if(!this->nextChars("\"", true)) return "";
    return string;
}

int JsonParser::parseNumber()
{
    QByteArray data;
    for(; this->itr != this->code.end() && *this->itr >= 48 && *this->itr <= 57; this->itr++) data += *this->itr;
    return data.isEmpty() ? 0 : data.toInt();
}
