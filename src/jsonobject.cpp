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
    for(QString pathElement : path.split(".")) {
        bool isInt;
        int iPathElement = pathElement.toInt(&isInt);
        if(isInt) {
            walk = &walk->element(iPathElement);
        } else {
            walk = &walk->element(pathElement);
        }
    }
    return *walk;
}

QString JsonObject::toJson()
{
    // simplify
    Type type = this->type();

    // generate base json
    QString json = this->typeTemplate();
    QString subJson;

    // handle null type
    if(type == Type::Null) return json;

    // handle string type
    else if(type == Type::String || type == Type::Number) {
        subJson = this->valueToJson();
    }

    // handle sub elements
    else {
        for(auto itr = this->objects.begin(); itr != this->objects.end(); itr++) {
            if(itr != this->objects.begin()) subJson += ", ";
            if(type == Object) {
                subJson += QString("\"%1\": %2").arg(itr.key(), itr.value().toJson());
            } else {
                subJson += itr.value().toJson();
            }
        }
    }
    return json.arg(subJson);
}

void JsonObject::fromJson(QByteArray json)
{
    JsonParser::parse(json, *this);
}

JsonObject::Type JsonObject::type()
{
    // check value
    if(!this->_value.isNull()) {
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

QString JsonObject::typeTemplate()
{
    switch(this->type()) {
        case Type::Array: return "[%1]";
        case Type::Object: return "{%1}";
        case Type::Null: return "null";
        default : return "%1";
    }
}

QString JsonObject::valueToJson()
{
    if(_value.type() == QVariant::String) goto string;
    if(_value.type() == QVariant::Bool) {
        return QString("%1").arg(_value.toBool() ? "true" : "false");
    }
    if(_value.canConvert<int>()) {
        return QString("%1").arg(_value.toInt());
    }
    string:
        return QString("\"%1\"").arg(this->_value.toString());
}

