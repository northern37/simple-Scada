#ifndef HISTORYRECORDER_H
#define HISTORYRECORDER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>

class QTimer;

// 监听 DataSource::valueProduced,异步批量写入 SQLite
// - 独立数据库连接(避免与全局默认连接冲突)
// - 内存缓冲 + 定时 flush,避免每条记录都执行 SQL
class HistoryRecorder : public QObject
{
    Q_OBJECT
public:
    explicit HistoryRecorder(QObject *parent = nullptr);
    ~HistoryRecorder() override;

    // 打开数据库:dbPath = ":memory:" 用内存库,否则存到文件
    bool open(const QString &dbPath = QStringLiteral(":memory:"));
    void close();

    // 取得内部数据库连接(供 HistoryPanel 共享)
    QSqlDatabase database() const { return mDb; }

    // 强制 flush 缓冲
    void flush();

    int recordCount();

public slots:
    // ★ 必须 public:外部 connect(DataSource::valueProduced, this, &HistoryRecorder::onValueProduced)
    void onValueProduced(const QString &name, const QVariant &value);

private slots:
    void onFlushTimer();

private:
    void ensureSchema();
    void insertOne(const QString &channel, double value, const QDateTime &ts);

    QSqlDatabase mDb;
    QString mConnName;
    QTimer *mFlushTimer = nullptr;

    struct PendingRecord {
        QString channel;
        double  value;
        qint64  epochMs;
    };
    QList<PendingRecord> mBuffer;
    static constexpr int kMaxBuffer = 50;     // 50 条立即 flush
};

#endif // HISTORYRECORDER_H
