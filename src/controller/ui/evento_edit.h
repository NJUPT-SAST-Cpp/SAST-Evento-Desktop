#ifndef EVENTO_EDIT_CONTROLLER_H
#define EVENTO_EDIT_CONTROLLER_H

#include <QtQml>

class DTO_Evento;

class EventoEditController : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(EventoEditController)
    QML_SINGLETON

    Q_PROPERTY(QString locationJson MEMBER m_locationJson NOTIFY locationJsonChanged)
    Q_PROPERTY(int index MEMBER m_index NOTIFY typeIdChanged)
    Q_PROPERTY(bool isEditMode MEMBER m_isEditMode NOTIFY isEditModeChanged)
    Q_PROPERTY(QString eventStart MEMBER m_eventStart NOTIFY eventStartChanged)
    Q_PROPERTY(QString eventEnd MEMBER m_eventEnd NOTIFY eventEndChanged)
    Q_PROPERTY(QString registerStart MEMBER m_registerStart NOTIFY registerStartChanged)
    Q_PROPERTY(QString registerEnd MEMBER m_registerEnd NOTIFY registerEndChanged)

public:
    Q_INVOKABLE void loadEditInfo();
    Q_INVOKABLE void createEvento(const QString& title, const QString& description,
                                  const QString& eventStart, const QString& eventEnd,
                                  const QString& registerStart, const QString& registerEnd,
                                  int index, int locationId, const QVariantList& departmentIds,
                                  const QString& tag);

private:
    QString m_locationJson;
    bool m_isEditMode; // true: 编辑模式 false: 创建模式
    // 编辑模式属性
    int m_index;
    QString m_eventStart;
    QString m_eventEnd;
    QString m_registerStart;
    QString m_registerEnd;

    void preload();

signals:
    void loadEditSuccessEvent();
    void loadEditErrorEvent(QString messge);

    void createSuccessEvent();
    void createErrorEvent(QString message);

    void departmentJsonChanged();
    void locationJsonChanged();
    void typeIdChanged();
    void isEditModeChanged();
    void eventStartChanged();
    void eventEndChanged();
    void registerStartChanged();
    void registerEndChanged();

private:
    EventoEditController() = default;

public:
    void update(const DTO_Evento& event);

    void onLoadEditFinished() {
        emit loadEditSuccessEvent();
    }
    void onLoadEditFailure(const QString& msg) {
        emit loadEditErrorEvent(msg);
    }

    void onCreateFinished() {
        emit createSuccessEvent();
    }
    void onCreateFailure(const QString& msg) {
        emit createErrorEvent(msg);
    }

    static EventoEditController* getInstance();
    static EventoEditController* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
};

#endif // EVENTO_EDIT_CONTROLLER_H
