# Json Qt Library

A very easy to use Qt library for Writing and Reading Json Data.

----------

### Example

Here is an [example](https://github.com/Spiek/jsonobject/blob/master/src/main.cpp) which should explain the basic use cases:

```c++
#include <QList>
#include <QMap>
#include <QString>
#include "jsonobject.h"

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
    qDebug("this.is.my.path = %d\n", main.path("this.is.my.path.0").integer());
	
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
```

This example produces the following result:
```
this.is.my.path = 12

Print Result of Main JsonObject:
{"array": ["Element 1", "Element 2", "Element 3", true], "integer": 12, "null": null, "object": {"Key 1": "Value 1", "Key 2": "Value 2", "key": "value"}, "string": "My String", "this": {"is": {"my": {"path": [12]}}}}

Print Result of Second JsonObject:
{"Second": {"array": ["Element 1", "Element 2", "Element 3", true], "integer": 12, "null": null, "object": {"Key 1": "Value 1", "Key 2": "Value 2", "key": "value"}, "string": "My String", "this": {"is": {"my": {"path": [12]}}}}}
```

### Compile Staticly:  
Just add the following to your Qt-Project file:
```qmake
include(Jsonobject.pri)
```
**Include project syntax:**  
#include "jsonobject.h"

----------

### Compile Dynamicly:   
Note: The **make install** installation pathes, are printed to you during qmake!
```
qmake Jsonobject
make
make install
```
add the following to your pro file:
```qmake
LIBS += -ljsonobject
```
**Include project syntax:**   
#include <jsonobject/jsonobject.h>

----------

### Licence
The [telegrambotlib-qt licence](https://github.com/Spiek/jsonobject/blob/master/LICENCE) is a modified version of the [LGPL](http://www.gnu.org/licenses/lgpl.html) licence, with a static linking exception.
