#include <QList>
#include <QMap>
#include <QString>
#include "jsonobject.h"

int main()
{
    JsonObject main;
    main.fromJson("{\"array\": [\"Element 1\", \"Element 2\", \"Element 3\", true], \"integer\": 12, \"null\": null, \"object\": {\"Key 1\": \"Value 1\", \"Key 2\": \"Value 2\", \"key\": \"value\"}, \"string\": \"My String\", \"this\": {\"is\": {\"my\": {\"path\": [12]}}}}");








    // array
    /*main["array"] = QList<QString> {"Element 1", "Element 2", "Element 3"};
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
    qDebug("this.is.my.path = %d\n", main.path("this.is.my.path.0").integer());*/
	
	// copy to other object
    JsonObject second;
    second["Second"] = main;
	
    // just print results
    for(int i = 0; i < 2; i++)
    {
        printf("Print Result of %s JsonObject:\n%s\n\n",
                    !i ? "Main" : "Second",
                    qPrintable((!i ? main : second).toJson()));
    }

    return 0;
}
