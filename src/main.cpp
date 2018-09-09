#include <QCoreApplication>
#include "jsonobject.h"
#include <QStringList>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    JsonObject object;
    object() = true;
    object()()()("Test") = 5.34f;

    printf(qPrintable(object.toJson()));
    printf("\n");
    fflush(stdout);

    JsonObject object2;
    object2.path("Bum") = QStringList {"dsa", "ds"};
    object2.path("Fisch") = QList<QString>{"halllo", "test"};
    object2.path("0.0.0.Test") = QMap<QString, QString>{{"0", "z"}, {"1", "s"}};

    printf(qPrintable(object2.toJson()));
    printf("\n");
    fflush(stdout);

    return 0;
}
