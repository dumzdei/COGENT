#pragma once
#include <QObject>
#include <QString>
#include <QAtomicInt>
#include "Parser.h"
#include "Exporter.h"

class CogentWorker : public QObject
{
    Q_OBJECT
public:
    explicit CogentWorker(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void startGeneration(const QString& inputDir, const QString& outputDir, const QString& format);
    void cancel();

signals:
    void progressChanged(int value, int max);
    void logMessage(const QString& msg);
    void finished(bool success, const QString& message);

private:
    QAtomicInt m_cancelRequested{ 0 };
};