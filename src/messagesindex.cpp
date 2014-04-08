#include "messagesindex.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QStringBuilder>
#include <QDateTime>
#include <QDebug>
#include <QImage>

#define ABLF_MAX 10000

MessagesIndex::MessagesIndex()
{
    m_indexPath = QDir::homePath() % QLatin1String(".litteras/index");
}

MessagesIndex::~MessagesIndex()
{
    if (m_rwXapianDB) {
        commit();
    }
}

void MessagesIndex::init()
{
    qWarning() << Q_FUNC_INFO;
    if (!open()) {
        return;
    }
}

void MessagesIndex::createMessage(const QVariantHash &data)
{

}

void MessagesIndex::markItem(const QString &filename)
{
//    QString uuid = uniqueId(filename);

//    Xapian::PostingIterator p = m_rwXapianDB->postlist_begin(uuid.toStdString());
//    if (p != m_rwXapianDB->postlist_end(uuid.toStdString())) {
//        Xapian::Document document = m_rwXapianDB->get_document(*p);

//        QDateTime dt = QDateTime::currentDateTime();
//        QString term("D");
//        term.append(dt.toString(Qt::ISODate));

//        document.add_boolean_term(term.toStdString());
//        m_rwXapianDB->replace_document(uuid.toStdString(), document);
//        qDebug() << term << document.termlist_count();

//        Xapian::TermIterator it = document.termlist_begin();
//        while (it != document.termlist_end()) {
////            qDebug() << QString::fromStdString(it.get_description()) << it.get_termfreq() << it.get_wdf() << QString::fromStdString(*it);
//            qDebug() << QString::fromStdString(*it);
//            ++it;
//        }
    //    }
}

bool MessagesIndex::open()
{
    QDir pathDir = m_indexPath;
    if (!pathDir.exists() && !pathDir.mkpath(m_indexPath)) {
        qWarning() << "Falha ao criar diretorio:" << m_indexPath;
        emit errorMessage(tr("Falha ao criar diretorio: %1").arg(m_indexPath));
        return false;
    }

    if (m_rwXapianDB == 0) {
        try {
            m_rwXapianDB = new Xapian::WritableDatabase(m_indexPath.toStdString(),
                                                        Xapian::DB_CREATE_OR_OPEN);
        } catch (const Xapian::Error &error) {
            qWarning() << "Exception: " << QString::fromStdString(error.get_msg());
            emit errorMessage(tr("Falha ao abrir índice: %1").arg(QString::fromStdString(error.get_msg())));
            return false;
        }
    }
    return true;
}

void MessagesIndex::commit()
{
    qDebug() << Q_FUNC_INFO;
    try {
        m_rwXapianDB->commit();
    } catch (const Xapian::Error &error) {
        qWarning() << "Error while commiting:" << QString::fromStdString(error.get_msg());
        emit errorMessage(tr("Erro ao persistir o banco de dados: %1").arg(QString::fromStdString(error.get_msg())));
    }
}

bool MessagesIndex::addFile(const QFileInfo &fileInfo, const QString &uuid)
{
//    CryptoFile file(fileInfo.absoluteFilePath().toLocal8Bit());
//    if (!file.open(CryptoFile::ReadOnly)) {
//        qWarning() << "Failed to open file while importing:" << fileInfo.fileName() << file.errorString();
//        return false;
//    }

//    Xapian::Document document;

//    QString id = uuid;
//    if (id.isNull()) {
//        id = uniqueId(fileInfo.absoluteFilePath());
//    }
//    // Add the unique id term
//    document.add_term(id.toStdString());

//    // Store the information about the file
//    const QString &absolutePath = fileInfo.absolutePath();
//    QString album = CryptoFile::albumSection(absolutePath);
//    QString artist = CryptoFile::artistSection(absolutePath);
//    QString title = fileInfo.completeBaseName();
//    document.add_value(AbsoluteFilePath, fileInfo.absoluteFilePath().toStdString());
//    document.add_value(AbsolutePath, absolutePath.toStdString());
//    document.add_value(AlbumDir, album.toStdString());
//    document.add_value(ArtistDir, artist.toStdString());
//    document.add_value(FileLastModified, QString::number(fileInfo.lastModified().toTime_t()).toStdString());
//    document.add_value(FileSize, QString::number(fileInfo.size()).toStdString());

//    if (file.mimeType().startsWith(QLatin1String("video"))) {
//        QFile dvd(absolutePath % QLatin1String("/.dvd"));
//        if (!dvd.exists()) {
//            dvd.open(QIODevice::WriteOnly);
//        }
//        document.add_value(IsVideo, "1");
//    } else {
//        document.add_value(IsVideo, "0");
//    }

//    // Check if the file belongs to the media from ABLF
//    if (absolutePath.startsWith(m_mediaABLF)) {
//        document.add_value(MediaFromABLF, "1");
//    } else {
//        document.add_value(MediaFromABLF, "0");
//    }

//    QHash<QString, QString> metadata = file.metadata();
//    if (!metadata[TAG_ALBUM].isEmpty()) {
//        album = metadata[TAG_ALBUM];
//    }
//    document.add_value(Album, album.toStdString());

//    if (!metadata[TAG_ARTIST].isEmpty()) {
//        artist = metadata[TAG_ARTIST];
//    }
//    document.add_value(Artist, artist.toStdString());

//    if (!metadata[TAG_TITLE].isEmpty()) {
//        title = metadata[TAG_TITLE];
//    } else {
//        title = fileInfo.baseName();
//    }
//    document.add_value(Title, title.toStdString());

//    uint year = metadata[TAG_YEAR].toInt();
//    document.add_value(AlbumYear, QString::number(year).toStdString());

//    uint trackNumber = metadata[TAG_TRACK].toInt();
//    document.add_value(TrackNumber, QString::number(trackNumber).toStdString());

//    QString genre = metadata[TAG_GENRE];
//    if (genre.isEmpty()) {
//        document.add_value(Genre, "Desconhecido");
//    } else {
//        document.add_value(Genre, genre.toStdString());
//    }

//    try {
//        // It's said that adding the docs sorted is faster.. (change in future)
//        m_rwXapianDB->replace_document(id.toStdString(), document);

//        return true;
//    } catch (const Xapian::Error &error) {
//        qWarning() << "Erro while adding documment: " << QString::fromStdString(error.get_msg());
//        emit errorMessage(tr("Erro ao adicionar documento: %1").arg(QString::fromStdString(error.get_msg())));
//        return false;
//    }
}

void MessagesIndex::createCoverFile(const QString &absolutePath, const QString &destFilePath)
{
    QStringList nameFilters;
    nameFilters << QLatin1String("*.png");
    nameFilters << QLatin1String("*.jpeg");
    nameFilters << QLatin1String("*.jpg");
    nameFilters << QLatin1String("*.pamp");

    QFileInfoList infoList;

    QDir dir(absolutePath);
    infoList = dir.entryInfoList(nameFilters,
                                 QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                 QDir::Size);
    foreach (const QFileInfo &fileInfo, infoList) {
        const QString &absoluteFilePath = fileInfo.absoluteFilePath();
        QImage image(fileInfo.absoluteFilePath());
        if (image.format() != QImage::Format_Invalid &&
                QFile::copy(absoluteFilePath, destFilePath)) {
            break;
        }
    }
}

bool MessagesIndex::hasDocument(const QString &uniqueId)
{
    // Try to find the item
    return m_rwXapianDB->postlist_begin(uniqueId.toStdString())
            != m_rwXapianDB->postlist_end(uniqueId.toStdString());
}
