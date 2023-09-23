#ifndef FEEDBACKNUMMODEL_H
#define FEEDBACKNUMMODEL_H

#include <QAbstractListModel>
#include <QtQml>

#include "types.h"

class FeedbackNumModel : public QAbstractListModel
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(FeedbackNumModel)

public:
    enum Role {
        EventId = Qt::DisplayRole + 1,
        Title,
        FeedbackCount
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void resetModel(std::vector<FeedbackNum> model);

    FeedbackNumModel(const FeedbackNumModel &) = delete;
    FeedbackNumModel &operator=(const FeedbackNumModel) = delete;

private:
    explicit FeedbackNumModel() = default;

    std::vector<FeedbackNum> m_data;

    std::mutex m_mutex;

public:
    static FeedbackNumModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
    static FeedbackNumModel *getInstance();
};

#endif // FEEDBACKNUMMODEL_H
