#include "HistoryRecorder.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QDebug>

HistoryRecorder::HistoryRecorder(QObject *parent) : QObject(parent)
{
    mFlushTimer = new QTimer(this);
    mFlushTimer->setInterval(2000);   // 2 秒强制 flush
    connect(mFlushTimer, &QTimer::timeout, this, &HistoryRecorder::onFlushTimer);
}

HistoryRecorder::~HistoryRecorder()
{
    flush();
    close();
}

bool HistoryRecorder::open(const QString &dbPath)
{
    // 用 this 指针做连接名后缀,保证多个实例不冲突
    mConnName = QStringLiteral("history_recorder_%1")
                    .arg(reinterpret_cast<quintptr>(this));
    mDb = QSqlDatabase::addDatabase("QSQLITE", mConnName);

    QString actualPath = dbPath;
    if (actualPath != ":memory:") {
        QFileInfo fi(actualPath);
        QDir().mkpath(fi.absolutePath());
    }
    mDb.setDatabaseName(actualPath);
    if (!mDb.open()) {
        qWarning() << "[HistoryRecorder] open failed:" << mDb.lastError().text();
        return false;
    }
    qDebug() << "[HistoryRecorder] opened at" << actualPath;
    ensureSchema();
    mFlushTimer->start();
    return true;
}

void HistoryRecorder::close()
{
    if (mDb.isOpen()) mDb.close();
    if (!mConnName.isEmpty() && QSqlDatabase::contains(mConnName)) {
        QSqlDatabase::removeDatabase(mConnName);
    }
    if (mFlushTimer) mFlushTimer->stop();
}

void HistoryRecorder::ensureSchema()
{
    QSqlQuery q(mDb);
    const QString ddl = R"SQL(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts TEXT NOT NULL,
            channel TEXT NOT NULL,
            value REAL NOT NULL
        )
    )SQL";
    if (!q.exec(ddl)) {
        qWarning() << "[HistoryRecorder] DDL failed:" << q.lastError().text();
    }
    q.exec("CREATE INDEX IF NOT EXISTS idx_history_ts ON history(ts DESC)");
    q.exec("CREATE INDEX IF NOT EXISTS idx_history_channel ON history(channel)");
}

void HistoryRecorder::onValueProduced(const QString &name, const QVariant &value)
{
    PendingRecord r;
    r.channel = name;
    r.value   = value.toDouble();
    r.epochMs = QDateTime::currentMSecsSinceEpoch();
    mBuffer.append(r);

    if (mBuffer.size() >= kMaxBuffer) {
        flush();
    }
}

void HistoryRecorder::onFlushTimer()
{
    flush();
}

void HistoryRecorder::flush()
{
    if (mBuffer.isEmpty() || !mDb.isOpen()) return;

    if (!mDb.transaction()) {
        qWarning() << "[HistoryRecorder] transaction failed:" << mDb.lastError().text();
        return;
    }

    QSqlQuery q(mDb);
    q.prepare("INSERT INTO history (ts, channel, value) VALUES (?, ?, ?)");
    for (const auto &r : std::as_const(mBuffer)) {
        const QString ts = QDateTime::fromMSecsSinceEpoch(r.epochMs)
                               .toString("yyyy-MM-dd HH:mm:ss.zzz");
        q.addBindValue(ts);
        q.addBindValue(r.channel);
        q.addBindValue(r.value);
        if (!q.exec()) {
            qWarning() << "[HistoryRecorder] insert failed:" << q.lastError().text();
            mDb.rollback();
            return;
        }
    }

    if (!mDb.commit()) {
        qWarning() << "[HistoryRecorder] commit failed:" << mDb.lastError().text();
        mDb.rollback();
        return;
    }
    mBuffer.clear();
}

int HistoryRecorder::recordCount()
{
    if (!mDb.isOpen()) return 0;
    QSqlQuery q(mDb);
    if (!q.exec("SELECT COUNT(*) FROM history")) return 0;
    if (q.next()) return q.value(0).toInt();
    return 0;
}
