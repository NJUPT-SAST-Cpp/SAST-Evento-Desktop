#ifndef EVENTO_BRIEF_MODEL_H
#define EVENTO_BRIEF_MODEL_H

#include <QAbstractListModel>
#include <QtQml>

#include "evento_brief.h"

class EventoBriefModel : public QAbstractListModel
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(EventoBriefModel)

public:
    enum Role
    {
        Id = Qt::DisplayRole + 1,
        Title,
        State,
        Description,
        Time,
        Url,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void resetModel(const std::vector<EventoBrief> &model);

    EventoBriefModel(const EventoBriefModel &) = delete;
    EventoBriefModel &operator=(const EventoBriefModel) = delete;

private:
    EventoBriefModel() = default;

    std::vector<EventoBrief> m_data;

    std::mutex m_mutex;

public:
    static EventoBriefModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
    static EventoBriefModel *getInstance();
};

#endif // EVENTO_BRIEF_MODEL_H
