#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QtGlobal>
#include <QSet>
#include <QHash>
#include <QStringList>

class JsonObject
{
    public:
        enum Type : char
        {
            Object,
            Array,
            String,
            Number
        };
        JsonObject() {}
        JsonObject(QString name, JsonObject* parent) : name(name), parentObject(parent) {}
        JsonObject* parent()
        {
            return this->parentObject;
        }

        JsonObject& path(QString path)
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

        JsonObject& element()
        {
            return this->element(this->index);
        }

        JsonObject& element(int index)
        {
            if(index >= this->index) this->index = index + 1;
            return this->element(QString::number(index));
        }

        JsonObject& element(QString index)
        {
            return this->objects.contains(index) ?
                            this->objects.find(index).value() :
                            this->objects.insert(index, JsonObject(index, this)).value();
        }

        JsonObject& operator()()
        {
            return this->element(this->index);
        }

        JsonObject& operator()(int index)
        {
            return this->element(index);
        }

        JsonObject& operator()(QString index)
        {
            return this->element(index);
        }

        template<typename T>
        JsonObject& operator=(T value)
        {
            this->value = qVariantFromValue<T>(value);
            return *this;
        }
        JsonObject& operator=(QStringList value)
        {
            *this = *dynamic_cast<QList<QString>*>(&value);
            return *this;
        }


        template<typename T>
        JsonObject& operator=(QList<T> value)
        {
            // add list to elements
            for(const T& t : value) {
                this->element() = qVariantFromValue<T>(t);
            }
            return *this;
        }

        template<typename T>
        JsonObject& operator=(QSet<T> value)
        {
            // add list to elements
            for(const T& t : value) {
                this->element() = qVariantFromValue<T>(t);
            }
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator=(QMap<K, T> value)
        {
            // add map to elements
            for(auto itr = value.begin(); itr != value.end(); itr++) {
                this->element(qVariantFromValue<K>(itr.key()).toString()) = qVariantFromValue<T>(itr.value());
            }
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator=(QHash<K, T> value)
        {
            // add map to elements
            for(auto itr = value.begin(); itr != value.end(); itr++) {
                this->element(qVariantFromValue<K>(itr.key()).toString()) = qVariantFromValue<T>(itr.value());
            }
            return *this;
        }

        QString toJson()
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

        Type type()
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


    private:
        QString typeTemplate()
        {
            switch(this->type()) {
                case Type::Array: return "[%1]";
                case Type::Object: return "{%1}";
               default : return "%1";
            }
        }


        QString valueToJson()
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

        int index = 0;
        QString name;
        QVariant value;
        QMap<QString, JsonObject> objects;
        JsonObject* parentObject = nullptr;
};

#endif // JSONOBJECT_H
