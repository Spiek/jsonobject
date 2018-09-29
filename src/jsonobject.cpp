#include "jsonobject.h"

JsonObject* JsonObject::parent()
{
    return this->parentObject;
}

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

JsonObject& JsonObject::path(QString path)
{
    JsonObject* walk = this;
    QString pathElement;
    for(auto itr = path.begin(); itr != path.end(); itr++) {
        // if we have not a finish element, just jump to next char
        bool isEnd = (itr + 1) == path.end();
        if(isEnd || (*itr != '.' && *itr != '/' && *itr != '\0')) {
            pathElement += *itr;
            if(!isEnd) continue;
        }

        // process empty path element
        if(pathElement.isEmpty()) {
            walk = &walk->element();
            continue;
        }

        // process path element
        bool isInt;
        int iPathElement = pathElement.toInt(&isInt);
        if(isInt) walk = &walk->element(iPathElement);
        else walk = &walk->element(pathElement);

        // clear path element
        pathElement.clear();
    }
    return *walk;
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
                json += QString("%1\"%2\": %3").arg(this->indentation(layer + 1), itr.key(), itr.value().toJsonImpl(style, layer + 1));
        }

        // handle array
        else if(style == Minimal) {
            json += itr.value().toJsonImpl(style, 0);
        } else {
            json += this->indentation(layer + 1) + itr.value().toJsonImpl(style, layer);
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
    // check value
    if(!this->_value.isNull()) {
        if(this->_value.type() == QVariant::Bool) return Type::Bool;
        return _value.canConvert<int>() ?
                    Type::Number :
                    Type::String;
    }

    // if we have no value set in object, we have an null type
    if(this->objects.isEmpty()) return Type::Null;

    // check sub value type
    bool isArray;
    int lastIndex = this->objects.lastKey().toInt(&isArray);
    isArray = isArray && this->objects.count() - 1 == lastIndex;
    return isArray ? Type::Array : Type::Object;
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

