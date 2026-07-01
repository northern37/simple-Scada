#include "IoSimulator.h"
#include <QRandomGenerator>
#include <QtMath>

IoSimulator::IoSimulator(QObject *parent) : DataSource(parent)
{
    // 默认 4 个通道(对应 4 类 SCADA 图元)
    // 调整原则:每个通道在 [0, 100] 范围内变化 40~70%,在 Linear/Knob 等图元上看明显
    mChannels.append({"temperature", 25.0, 50.0, 10.0});  // 温度:baseline=50, ±25,范围 [25, 75]
    mChannels.append({"pressure",    30.0, 50.0,  8.0});  // 压力:baseline=50, ±30,范围 [20, 80]
    mChannels.append({"level",       30.0, 60.0, 12.0});  // 液位:baseline=60, ±30,范围 [30, 90]
    mChannels.append({"flow",        25.0, 50.0,  6.0});  // 流量:baseline=50, ±25,范围 [25, 75] 避免负数

    mTimer.setInterval(mIntervalMs);
    connect(&mTimer, &QTimer::timeout, this, &IoSimulator::onTick);
}

void IoSimulator::start()
{
    mTick = 0;
    mTimer.start();
}

void IoSimulator::stop()
{
    mTimer.stop();
}

void IoSimulator::onTick()
{
    mTick = (mTick + 1) % 10000;   // ★ 防止 t 无限增长导致 double 精度损失
    const double t = mTick * (mIntervalMs / 1000.0);

    for (const Channel &ch : std::as_const(mChannels)) {
        const double noise = QRandomGenerator::global()->bounded(100) / 100.0 - 0.5;
        const double value = ch.baseline
                           + ch.amplitude * qSin(2.0 * M_PI * t / ch.period)
                           + noise;
        emit valueProduced(ch.name, QVariant(value));
    }
}
