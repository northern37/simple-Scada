#include "IoSimulator.h"
#include <QRandomGenerator>
#include <QtMath>

IoSimulator::IoSimulator(QObject *parent) : DataSource(parent)
{
    // 默认 4 个通道(对应 4 类 SCADA 图元)
    // 振幅加大使波形能跨越 Warning/Critical 阈值,展示报警功能
    // temperature: 范围 ≈ [-5, 105] → 可触发 Warning>80 及 Critical>100
    // pressure:    范围 ≈ [0,  100] → 可触发 Warning>75 及 Critical>90
    // level:       范围 ≈ [0,  100] → 可触发 Warning>80 及 Critical>95
    // flow:        范围 ≈ [0,  100] → 可触发 Warning>70 及 Critical>90
    mChannels.append({"temperature", 55.0, 50.0,  8.0});
    mChannels.append({"pressure",    50.0, 50.0, 10.0});
    mChannels.append({"level",       50.0, 50.0, 12.0});
    mChannels.append({"flow",        50.0, 50.0,  6.0});

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
