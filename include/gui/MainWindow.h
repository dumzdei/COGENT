#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QThread>

class CogentWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onBrowseInput();
    void onBrowseOutput();
    void onGenerate();
    void onCancel();
    void onWorkerLog(const QString& msg);
    void onWorkerProgress(int current, int total);
    void onWorkerFinished(bool success, const QString& msg);

private:
    void setupUI();
    void setupConnections();
    void setUIEnabled(bool enabled);

    QLineEdit* m_inputPathEdit;
    QLineEdit* m_outputPathEdit;
    QComboBox* m_formatCombo;
    QPushButton* m_btnGenerate;
    QPushButton* m_btnCancel;
    QTextEdit* m_logEdit;
    QProgressBar* m_progressBar;

    QThread* m_workerThread;
    CogentWorker* m_worker;
};