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

class JsonParser;
class JsonParseResult;
class JsonObject
{
    public:
        // Json value type
        enum Type : char
        {
            Object,
            Array,
            String,
            Number,
            Bool,
            Null
        };

        // Json generation style
        enum Style : char
        {
            Minimal,
            Pretty
        };

        // construct
        JsonObject(JsonObject *parent = nullptr) : parentObject(parent) {}
        JsonObject(const JsonObject &src)
        {
            this->_index = src._index;
            this->_value = src._value;
            this->objects = src.objects;
        }
        JsonObject(JsonObject&& move)
        {
            this->_index = std::move(move._index);
            this->_value = std::move(move._value);
            this->objects = std::move(move.objects);
        }

        // element accsss function
        JsonObject* parent();
        JsonObject& element();
        JsonObject& element(int index);
        JsonObject& element(QString index);
        JsonObject* path(QString path, bool createPath = true);
        inline JsonObject* path(QStringList path, bool createPath = true) { return this->path(path.join(static_cast<QChar>('\0')), createPath); }
        inline int count() { return this->objects.count(); }

        // element remove functions
        inline bool remove(int index) { return this->remove(QString::number(index)); }
        inline bool remove(QString index) { return this->objects.remove(index) == 1; }
        inline JsonObject& removed(int index) { this->remove(index); return *this; }
        inline JsonObject& removed(QString index) { this->remove(index); return *this; }

        // element contains functions
        inline bool contains(int index) { return this->contains(QString::number(index)); }
        inline bool contains(QString index) { return this->objects.contains(index); }
        inline bool containsPath(QString path) { return this->path(path, false); }

        // operators
        inline JsonObject& operator()() { return this->element(this->_index); }
        inline JsonObject& operator()(int index) { return this->element(index); }
        inline JsonObject& operator()(QString index) { return this->element(index); }
        inline JsonObject& operator[](int index) { return this->element(index); }
        inline JsonObject& operator[](QString index) { return this->element(index); }

        // parsing
        inline QString toJson(Style style = Minimal) { return this->toJsonImpl(style, 0); }
        JsonParseResult fromJson(const QByteArray &json);

        // internal
        Type type();

        // data
        inline bool isNull() { return this->type() == Type::Null; }
        inline QVariant& value() { return this->_value; }
        template<typename T>
        T value(bool* success = nullptr)
        {
            if(success) *success = this->_value.canConvert<T>();
            return this->_value.value<T>();
        }
        inline int& index() { return this->_index; }

        // data simplifier
        inline bool boolean(bool* success = nullptr) { return this->value<bool>(success); }
        inline int integer(bool* success = nullptr) { return this->value<int>(success); }
        inline QString string(bool* success = nullptr) { return this->value<QString>(success); }

        ///
        /// operator= Templates
        ///
        template<typename T>
        JsonObject& operator=(const T& value)
        {
            this->_value = qVariantFromValue<T>(value);
            return *this;
        }
        JsonObject& operator=(const char* value)
        {
            this->_value = qVariantFromValue<QString>(QString(value));
            return *this;
        }
        JsonObject& operator=(const QStringList& value)
        {
            // QStringlist will be handled as QList<QString>
            *this = *dynamic_cast<const QList<QString>*>(&value);
            return *this;
        }
        JsonObject& operator=(JsonObject& value)
        {
            // if necessary, update this index
            if(this->index() < value.index()) this->_index = value.index();
            this->_value = value._value;
            this->objects = value.objects;
            return *this;
        }

        // template
        template<typename T>
        JsonObject& operator=(QList<T> value)
        {
            this->objects.clear();
            *this += value;
            return *this;
        }

        template<typename T>
        JsonObject& operator=(QSet<T> value)
        {
            this->objects.clear();
            *this += value;
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator=(QMap<K, T> value)
        {
            this->objects.clear();
            *this += value;
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator=(QHash<K, T> value)
        {
            this->objects.clear();
            *this += value;
            return *this;
        }

        ///
        /// operator+= Templates
        ///
        template<typename T>
        JsonObject& operator+=(QList<T> value)
        {
            // add list to elements
            this->_value.clear();
            for(const T& t : value) {
                this->element() = qVariantFromValue<T>(t);
            }
            return *this;
        }

        template<typename T>
        JsonObject& operator+=(QSet<T> value)
        {
            // add list to elements
            this->_value.clear();
            for(const T& t : value) {
                this->element() = qVariantFromValue<T>(t);
            }
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator+=(QMap<K, T> value)
        {
            // add map to elements
            this->_value.clear();
            for(auto itr = value.begin(); itr != value.end(); itr++) {
                this->element(qVariantFromValue<K>(itr.key()).toString()) = qVariantFromValue<T>(itr.value());
            }
            return *this;
        }

        template<typename K, typename T>
        JsonObject& operator+=(QHash<K, T> value)
        {
            // add map to elements
            this->_value.clear();
            for(auto itr = value.begin(); itr != value.end(); itr++) {
                this->element(qVariantFromValue<K>(itr.key()).toString()) = qVariantFromValue<T>(itr.value());
            }
            return *this;
        }

        // Element looping (STL Container)
        inline QMap<QString, JsonObject>::iterator begin() { return this->objects.begin(); }
        inline QMap<QString, JsonObject>::const_iterator begin() const { return this->objects.begin(); }
        inline QMap<QString, JsonObject>::const_iterator constBegin() const { return this->objects.constBegin(); }
        inline QMap<QString, JsonObject>::const_iterator cbegin() const { return this->objects.cbegin(); }
        inline QMap<QString, JsonObject>::iterator end() { return this->objects.end(); }
        inline QMap<QString, JsonObject>::const_iterator end() const { return this->objects.end(); }
        inline QMap<QString, JsonObject>::const_iterator constEnd() const { return this->objects.constEnd(); }
        inline QMap<QString, JsonObject>::const_iterator cend() const { return this->objects.cend(); }
        inline QMap<QString, JsonObject>::iterator erase(QMap<QString, JsonObject>::iterator it) { return this->objects.erase(it); }

    private:
        // helpers
        inline QString indentation(int layer) { return QString("    ").repeated(layer); }
        QString toJsonImpl(Style style, int layer = 0);
        QString valueToJson();

        // internal data
        int _index = 0;
        QVariant _value;
        QMap<QString, JsonObject> objects;
        JsonObject* parentObject = nullptr;
};
#include "jsonparser.h"
Q_DECLARE_METATYPE(JsonObject)

#endif // JSONOBJECT_H
