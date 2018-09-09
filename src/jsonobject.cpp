#include "jsonobject.h"

JsonObject* JsonObject::parent()
{
    return this->parentObject;
}

JsonObject& JsonObject::element()
{
    return this->element(this->index);
}

JsonObject& JsonObject::element(int index)
{
    if(index >= this->index) this->index = index + 1;
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

    // handle string type
    if(type == Type::String || type == Type::Number) {
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

JsonObject::Type JsonObject::type()
{
    // check value
    if(!this->value.isNull()) {
        return value.canConvert<int>() ?
                    Type::Number :
                    Type::String;
    }

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
       default : return "%1";
    }
}


QString JsonObject::valueToJson()
{
    if(value.type() == QVariant::String) goto string;
    if(value.type() == QVariant::Bool) {
        return QString("%1").arg(value.toBool() ? "true" : "false");
    }
    if(value.canConvert<int>()) {
        return QString("%1").arg(value.toInt());
    }
    string:
        return QString("\"%1\"").arg(this->value.toString());
}

