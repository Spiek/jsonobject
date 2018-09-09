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

        // construct
        JsonObject(JsonObject *parent = nullptr) : parentObject(parent) {}

        // element function
        JsonObject* parent();
        JsonObject& element();
        JsonObject& element(int index);
        JsonObject& element(QString index);
        JsonObject& path(QString path);

        // operators
        inline JsonObject& operator()() { return this->element(this->index); }
        inline JsonObject& operator()(int index) { return this->element(index); }
        inline JsonObject& operator()(QString index) { return this->element(index); }
        inline JsonObject& operator[](int index) { return this->element(index); }
        inline JsonObject& operator[](QString index) { return this->element(index); }

        // parsing
        QString toJson();

        // internal
        Type type();

        // templates
        template<typename T>
        JsonObject& operator=(T value)
        {
            this->value = qVariantFromValue<T>(value);
            return *this;
        }
        JsonObject& operator=(QStringList value)
        {
            // QStringlist will be handled as QList<QString>
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

    private:
        // helpers
        QString typeTemplate();
        QString valueToJson();

        // internal data
        int index = 0;
        QVariant value;
        QMap<QString, JsonObject> objects;
        JsonObject* parentObject = nullptr;
};

#endif // JSONOBJECT_H
