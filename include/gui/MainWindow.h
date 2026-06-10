#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include "CogentWorker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onActionCreate();
    void onActionSaveAs();
    void onBrowseClicked();
    void onRunClicked();
    void onWorkerLog(const QString& message);
    void onWorkerFinished(bool success, const QString& resultMessage);

private:
    QAction* actionCreate;
    QAction* actionSaveAs;

    QPlainTextEdit* editor;
    QLineEdit* pathEdit;
    QPushButton* browseBtn;
    QComboBox* formatCombo;
    QPushButton* runBtn;
    QTextEdit* logEdit;

    QThread* workerThread;
    CogentWorker* worker;
};