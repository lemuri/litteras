#ifndef MESSAGESINDEX_H
#define MESSAGESINDEX_H

#include <QObject>
#include <QStringList>
#include <QFileInfo>
#include <QTimer>

#undef slots
#include <xapian.h>
#define slots

class Message
{
public:
    QString accountId;
    QString messageId;
    QString from;
    QString to;
};

class MessagesIndex : public QObject
{
    Q_OBJECT
public:
    enum XapianRoles {
        AccountId,
        MessageId,
        From,
        To,
        Cc,
        Cco,
        Subject,
        Size,
        DateTime,
        Read,
        Favourite
    };
    MessagesIndex();
    ~MessagesIndex();

signals:
    void errorMessage(const QString &text);
    void changed();
    void finished();

public slots:
    void init();
    void createMessage(const QVariantHash &data);
    void markItem(const QString &filename);

private:
    bool open();
    void commit();
    bool hasDocument(const QString &uniqueId);
    bool addFile(const QFileInfo &fileInfo, const QString &uuid = QString());
    void createCoverFile(const QString &absolutePath, const QString &destFilePath);

    Xapian::WritableDatabase *m_rwXapianDB = 0;
    QString m_indexPath;
};

#endif // MessagesIndex_H
