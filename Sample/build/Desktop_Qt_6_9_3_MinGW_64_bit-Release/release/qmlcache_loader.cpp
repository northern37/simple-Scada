#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

namespace QmlCacheGeneratedCode {
namespace _qsimplescada_widgets_EEIoT_Linear_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_widgets_EEIoT_Knob_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_widgets_EEIoT_WaterLevel_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_extensions_ValueLabel_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_extensions_TrendChart_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_extensions_PieChart_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qsimplescada_extensions_BarGraph_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    ~Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry)


Registry::Registry() {
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/widgets/EEIoT/Linear.qml"), &QmlCacheGeneratedCode::_qsimplescada_widgets_EEIoT_Linear_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/widgets/EEIoT/Knob.qml"), &QmlCacheGeneratedCode::_qsimplescada_widgets_EEIoT_Knob_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/widgets/EEIoT/WaterLevel.qml"), &QmlCacheGeneratedCode::_qsimplescada_widgets_EEIoT_WaterLevel_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/extensions/ValueLabel.qml"), &QmlCacheGeneratedCode::_qsimplescada_extensions_ValueLabel_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/extensions/TrendChart.qml"), &QmlCacheGeneratedCode::_qsimplescada_extensions_TrendChart_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/extensions/PieChart.qml"), &QmlCacheGeneratedCode::_qsimplescada_extensions_PieChart_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qsimplescada/extensions/BarGraph.qml"), &QmlCacheGeneratedCode::_qsimplescada_extensions_BarGraph_qml::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.structVersion = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

Registry::~Registry() {
    QQmlPrivate::qmlunregister(QQmlPrivate::QmlUnitCacheHookRegistration, quintptr(&lookupCachedUnit));
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_widgets)() {
    ::unitRegistry();
    Q_INIT_RESOURCE(___QSimpleScadaWidgets_widgets_qmlcache);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_widgets))
int QT_MANGLE_NAMESPACE(qCleanupResources_widgets)() {
    Q_CLEANUP_RESOURCE(___QSimpleScadaWidgets_widgets_qmlcache);
    return 1;
}
int QT_MANGLE_NAMESPACE(qInitResources_extensions)() {
    ::unitRegistry();
    Q_INIT_RESOURCE(___QSimpleScadaExtensions_extensions_qmlcache);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_extensions))
int QT_MANGLE_NAMESPACE(qCleanupResources_extensions)() {
    Q_CLEANUP_RESOURCE(___QSimpleScadaExtensions_extensions_qmlcache);
    return 1;
}
