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

QString JsonObject::toJson()
{
    // simplify
    Type type = this->type();

    // generate json for primitive types
    if(type != Type::Object && type != Type::Array) {
        return this->valueToJson();
    }

    // generate json for object/array
    else {
        QString json;
        for(auto itr = this->objects.begin(); itr != this->objects.end(); itr++) {
            if(itr != this->objects.begin()) json += ",";
            if(type == Object) {
                json += QString("\"%1\":%2").arg(itr.key(), itr.value().toJson());
            } else {
                json += itr.value().toJson();
            }
        }
        return type == Object ? "{" + json + "}" : "[" + json + "]";
    }
}

void JsonObject::fromJson(QByteArray json)
{
    JsonParser::parse(json, *this);
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

