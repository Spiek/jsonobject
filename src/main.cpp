#include <QList>
#include <QMap>
#include <QString>

#include "jsonobject.h"

void print(QString text, JsonObject& data)
{
    printf("\n");
    printf(qPrintable(text));
    printf("\n");
    printf(qPrintable(data.toJson()));
    printf("\n");
    fflush(stdout);
}

int main()
{
    JsonObject main;

    // array
    main["array"] = QList<QString> {"Element 1", "Element 2", "Element 3"};
    main["array"]() = true;

    // object
    main["object"] = QMap<QString, QString> {{"Key 1", "Value 1"}, {"Key 2", "Value 2"}};
    main["object"]["key"] = "value";

    // integer
    main["integer"] = 12;

    // string
    main["string"] = "My String";

    // null
    main["null"];

    // set via path
    main.path("this.is.my.path.0") = 12;

    // access value
    qDebug("this.is.my.path = %d", main.path("this.is.my.path").integer());

    // print result
    print("Main Object:", main);

    // copy to other object
    JsonObject second;
    second["Second"] = main;
    print("Second Object:", second);

    return 0;
}
