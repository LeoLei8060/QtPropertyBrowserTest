#include "widget.h"
#include "./ui_widget.h"

#include <QDebug>
#include <QMetaProperty>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_data(new Derived(this))
{
    ui->setupUi(this);

    initUI();
    initPropertiesManagerForObject();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUI()
{
    // 初始化ListItem的用户数据
    QObject *objs[2] = {this, m_data};
    for (int i = 0; i < 2; ++i) {
        ui->listWidget->item(i)->setData(Qt::UserRole, QVariant::fromValue(objs[i]));
    }

    // 初始化属性栏
    ui->propertyBrowser->setIndentation(20);            // 树节点的缩进量
    ui->propertyBrowser->setRootIsDecorated(true);      // 是否显示根节点前是否显示装饰（加减号）
    ui->propertyBrowser->setAlternatingRowColors(true); // 行是否使用交替色
    ui->propertyBrowser->setHeaderVisible(true);        // 是否显示表头
    // 设置列的调整模式:
    //  Interactive(用户可以调整列宽)
    //  Stretch(列宽度自动伸展以填满可用空间)
    //  Fixed(列宽固定)
    //  ResizeToContents(列宽自动调整以适应内容)
    ui->propertyBrowser->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);

    connect(m_data, &Derived::countChanged, this, [=](int val) {
        ui->listWidget_2->addItem(QString("count changed: %1").arg(val));
    });
    connect(m_data, &Derived::nameChanged, this, [=](const QString &val) {
        ui->listWidget_2->addItem(QString("name changed: %1").arg(val));
    });
    connect(m_data, &Derived::describeChanged, this, [=](const QString &val) {
        ui->listWidget_2->addItem(QString("describe changed: %1").arg(val));
    });
    connect(m_data, &Derived::typeChanged, this, [=](int val) {
        ui->listWidget_2->addItem(QString("type changed: %1").arg(val));
    });
}

void Widget::initPropertiesManagerForObject()
{
    auto initPropertyimpl = [this](QObject *object, QObject *layout = nullptr) {
        auto factor = new QtVariantEditorFactory();
        auto manager = new QtVariantPropertyManager();

        connect(manager,
                &QtVariantPropertyManager::valueChanged,
                this,
                [this, manager, object](QtProperty *property, const QVariant &value) {
                    // 获取属性名并更新对象的对应属性
                    for (int i = 0; i < object->metaObject()->propertyCount(); ++i) {
                        QMetaProperty metaProperty = object->metaObject()->property(i);
                        if (property->propertyName() == QString(metaProperty.name())) {
                            object->setProperty(metaProperty.name(), value);
                            break;
                        }
                    }
                });

        auto objItem = manager->addProperty(QtVariantPropertyManager::groupTypeId(), tr("Object"));
        auto objNameItem = manager->addProperty(QVariant::String, tr("ObjectName"));
        objItem->addSubProperty(objNameItem);
        auto metaObj = object->metaObject();
        auto styleItem = manager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                              metaObj->superClass()->className());
        for (int i = 1; i < metaObj->propertyCount(); ++i) {
            auto    property = metaObj->property(i);
            QString propertyName = property.name();
            auto    item = manager->addProperty(property.type(), propertyName);
            styleItem->addSubProperty(item);
        }
        if (layout) {
            auto layoutItem = manager->addProperty(QtVariantPropertyManager::groupTypeId(), "Layout");
            auto layoutMetaObj = layout->metaObject();
            for (int i = 1; i < layoutMetaObj->propertyCount(); ++i) {
                auto    property = layoutMetaObj->property(i);
                QString propertyName = property.name();
                auto    item = manager->addProperty(property.type(), propertyName);
                layoutItem->addSubProperty(item);
            }

            m_propertiesItem[metaObj->className()] = {objItem, styleItem, layoutItem};
        } else
            m_propertiesItem[metaObj->className()] = {objItem, styleItem};

        m_propertiesManager[metaObj->className()] = manager;
        ui->propertyBrowser->setFactoryForManager(manager, factor);
    };

    initPropertyimpl(this, this->layout());
    initPropertyimpl(m_data);
}

void Widget::updateProperties(QObject *selectedObject)
{
    ui->propertyBrowser->clear();

    auto metaObj = selectedObject->metaObject();
    qDebug() << metaObj->className();
    auto manager = m_propertiesManager[metaObj->className()];

    auto properties = m_propertiesItem[metaObj->className()];
    for (auto item : properties) {
        for (auto subItem : item->subProperties()) {
            if (subItem->propertyName() == "ObjectName")
                manager->setValue(subItem, metaObj->className());
            else {
                for (int i = 0; i < metaObj->propertyCount(); ++i) {
                    if (metaObj->property(i).name() == subItem->propertyName()) {
                        QVariant val = selectedObject->property(metaObj->property(i).name());
                        if (val.isValid())
                            manager->setValue(subItem, val);
                        break;
                    }
                }
            }
        }

        ui->propertyBrowser->addProperty(item);
    }
}

void Widget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    auto object = current->data(Qt::UserRole).value<QObject *>();
    if (object)
        updateProperties(object);
}
