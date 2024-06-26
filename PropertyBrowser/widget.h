#ifndef WIDGET_H
#define WIDGET_H

#include "Data.h"
#include "qtpropertybrowser/qteditorfactory.h"
#include "qtpropertybrowser/qttreepropertybrowser.h"
#include "qtpropertybrowser/qtvariantproperty.h"

#include <QListWidgetItem>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void initUI();
    void initPropertiesManagerForObject();
    void updateProperties(QObject *selectedObject);

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::Widget *ui;

    QMap<QString, QtVariantPropertyManager *> m_propertiesManager;
    QMap<QString, QList<QtProperty *>>        m_propertiesItem;

    Derived *m_data;
};
#endif // WIDGET_H
