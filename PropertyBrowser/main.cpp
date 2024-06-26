#include "widget.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载样式表
    QFile   file("://res/style.css");
    QString stylesheet;
    if (file.open(QFile::ReadOnly)) {
        stylesheet = QLatin1String(file.readAll());
        a.setStyleSheet(stylesheet);
        file.close();
    }

    Widget w;
    w.show();
    return a.exec();
}
