#ifndef PLAZA_CONTROLLER_H
#define PLAZA_CONTROLLER_H

#include "evento_exception.h"

#include <QtQml>

class PlazaController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PlazaController)
    QML_SINGLETON

public:
    Q_INVOKABLE void loadPlazaInfo();
    Q_INVOKABLE void openUrl(const QString& link);

signals:
    void loadPlazaSuccessEvent();
    void loadPlazaErrorEvent(const QString message);
public:
    inline void onPlazaLoadFailure(const EventoException& err) {
        emit loadPlazaErrorEvent(err.message());
    }
    inline void onPlazaLoadFinished() {
        emit loadPlazaSuccessEvent();
    }
private:
    PlazaController() = default;
public:
    static PlazaController *getInstance();
    static PlazaController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
};

#endif // PLAZA_CONTROLLER_H
