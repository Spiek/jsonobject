#include "jsonobject.h"

JsonObject& JsonObject::element()
{
    return this->element(this->_index);
}

JsonObject& JsonObject::element(int index)
{
    if(index < 0) index = qMax(0, this->_index + index);
    if(index >= this->_index) this->_index = index + 1;
    return this->element(QString::number(index));
}

JsonObject& JsonObject::element(QString index)
{
    return this->objects.contains(index) ?
                    this->objects.find(index).value() :
                    this->objects.insert(index, JsonObject(this)).value();
}

JsonObject* JsonObject::pathImpl(QString path, bool createPath, bool reverse)
{
    JsonObject* walk = this;
    QString pathElement;
    for(auto itr = path.begin(); walk && itr != path.end(); itr++) {
        // if we have not a finish element, just jump to next char
        bool isEnd = (itr + 1) == path.end();
        bool isDelimiter = *itr == '.' || *itr == '/' || *itr == '\0';
        if(isEnd || !isDelimiter) {
            if(!isDelimiter) pathElement += *itr;
            if(!isEnd) continue;
        }

        // process empty path element
        if(pathElement.isEmpty()) {
            if(reverse) {
                if(!walk->parent() && createPath) walk->parentObject = new JsonObject;
                walk = walk->parent();
            }
            else if(createPath) walk = &walk->element();
            continue;
        }

        // process path element
        bool isInt;
        int iPathElement = pathElement.toInt(&isInt);
        if(isInt) {
            if(reverse) {
                if(!walk->parent() && createPath) walk->parentObject = new JsonObject;
                if(!walk->parent()) return nullptr;
                if(walk->parent()->contains(iPathElement) || createPath) walk = &walk->parent()->element(iPathElement);
            }
            else if(!createPath && !walk->contains(iPathElement)) return nullptr;
            else walk = &walk->element(iPathElement);
        } else {
            if(reverse) {
                if(!walk->parent() && createPath) walk->parentObject = new JsonObject;
                if(!walk->parent()) return nullptr;
                if(walk->parent()->contains(pathElement) || createPath) walk = &walk->parent()->element(pathElement);
            }
            else if(!createPath && !walk->contains(pathElement)) return nullptr;
            else walk = &walk->element(pathElement);
        }

        // clear path element
        pathElement.clear();
    }
    return walk;
}

QString JsonObject::toJsonImpl(Style style, int layer)
{
    // simplify
    Type type = this->type();

    // generate json for primitive types
    if(type != Type::Object && type != Type::Array) {
        if(style == Minimal) return this->valueToJson();
        return this->indentation(this->parent() ? layer : 0) + this->valueToJson();
    }

    // generate json for objects/arrays
    QString json;
    for(auto itr = this->objects.begin(); itr != this->objects.end(); itr++) {
        if(!json.isEmpty()) json += style == Minimal ? "," : ",\n";
        // handle object
        if(type == Object) {
            if(style == Minimal)
                json += QString("\"%1\":%2").arg(itr.key(), itr.value().toJsonImpl(style, 0));
            else
                json += QString("%1\"%2\": %3").arg(this->indentation(layer + 1), itr.key(), itr.value().toJsonImpl(style, itr.value().isSimpleType() ? 0 : layer + 1));
        }

        // handle array
        else if(style == Minimal) {
            json += itr.value().toJsonImpl(style, 0);
        } else {
            json += this->indentation(layer + 1) + itr.value().toJsonImpl(style, itr.value().isSimpleType() ? 0 : layer + 1);
        }
    }

    // generate container
    QString border = type == Object ? "{}" : "[]";
    if(style == Minimal) return border[0] + json + border[1];
    return QString(border[0] + "\n%1\n%2" + border[1]).arg(json, this->indentation(layer));
}

JsonParseResult JsonObject::fromJson(const QByteArray &json)
{
    return JsonParser::parse(json, *this);
}

JsonObject::Type JsonObject::type()
{
    // check sub value type
    if(!this->objects.isEmpty()) {
        bool isArray;
        int lastIndex = this->objects.lastKey().toInt(&isArray);
        isArray = isArray && this->objects.count() - 1 == lastIndex;
        return isArray ? Type::Array : Type::Object;
    }

    // check value
    if(!this->_value.isNull()) {
        if(this->_value.type() == QVariant::Bool) return Type::Bool;
        return _value.canConvert<int>() ?
                    Type::Number :
                    Type::String;
    }

    // here we have a null type
    return Type::Null;
}

QString JsonObject::valueToJson()
{
    if(_value.isNull()) return "null";
    if(_value.type() == QVariant::String) goto string;
    if(_value.type() == QVariant::Bool) {
        return _value.toBool() ? "true" : "false";
    }
    if(_value.canConvert<int>()) {
        return QString::number(_value.toInt());
    }
    string:
        return QString("\"%1\"").arg(this->_value.toString());
}

