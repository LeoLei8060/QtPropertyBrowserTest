#ifndef DATA_H
#define DATA_H

#include <QObject>

class Base : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count MEMBER m_count NOTIFY countChanged FINAL)
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged FINAL)
public:
    explicit Base(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

signals:
    void countChanged(int);
    void nameChanged(const QString&);

private:
    int m_count { 10 };
    QString m_name { "Base" };
};

class Derived : public Base {
    Q_OBJECT
    Q_PROPERTY(int type MEMBER m_type NOTIFY typeChanged FINAL)
    Q_PROPERTY(QString describe MEMBER m_describe NOTIFY describeChanged FINAL)
public:
    explicit Derived(QObject* parent = nullptr)
        : Base(parent)
    {
    }

signals:
    void typeChanged(int);
    void describeChanged(const QString&);

private:
    int m_type { 2 };
    QString m_describe { "派生类" };
};

#endif // DATA_H
