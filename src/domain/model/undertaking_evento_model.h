#ifndef UNDERTAKING_EVENTO_MODEL_H
#define UNDERTAKING_EVENTO_MODEL_H

#include <QAbstractListModel>
#include <QtQml>

#include "undertaking_evento.h"

class UndertakingEventoModel : public QAbstractListModel
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(UndertakingEventoModel)

public:
    enum Role {
        Id = Qt::DisplayRole + 1,
        Title,
        Time,
        Location,
        Department,
        Url,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void resetModel(const std::vector<UndertakingEvento>& model);

private:
    UndertakingEventoModel() = default;

    std::vector<UndertakingEvento> m_data;

    std::mutex m_mutex;
public:
    static UndertakingEventoModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        return getInstance();
    }
    inline static UndertakingEventoModel *getInstance()
    {
        static UndertakingEventoModel singleton;
        return &singleton;
    }
};

#endif // UNDERTAKING_EVENTO_MODEL_H
