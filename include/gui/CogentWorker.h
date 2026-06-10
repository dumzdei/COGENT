#pragma once

#include <QObject>
#include <QString>
#include <filesystem>
#include "Parser.h"
#include "Exporter.h"

class CogentWorker : public QObject {
    Q_OBJECT

public:
    explicit CogentWorker(QObject* parent = nullptr);
    ~CogentWorker();

public slots:
    void process(const QString& pathStr, int formatInt);

signals:
    void logMessage(const QString& message);
    void finished(bool success, const QString& resultMessage);
};