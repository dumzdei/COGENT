#include "gui/MainWindow.h"
#include "gui/CogentWorker.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QLabel>
#include <QGroupBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_workerThread(new QThread(this)), m_worker(nullptr)
{
    setWindowTitle("COGENT GUI");
    resize(900, 650);
    setupUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    if (m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
    delete m_worker;
}

void MainWindow::setupUI()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);

    // === Параметры ===
    auto* paramsBox = new QGroupBox("⚙️ Параметры генерации", this);
    auto* paramsLayout = new QGridLayout(paramsBox);

    paramsLayout->addWidget(new QLabel("Входная папка:"), 0, 0);
    m_inputPathEdit = new QLineEdit(this);
    m_inputPathEdit->setPlaceholderText("Выберите папку с HDL исходниками...");
    paramsLayout->addWidget(m_inputPathEdit, 0, 1);

    auto* btnBrowseIn = new QPushButton("📂", this);
    btnBrowseIn->setMaximumWidth(40);
    paramsLayout->addWidget(btnBrowseIn, 0, 2);

    paramsLayout->addWidget(new QLabel("Выходная папка:"), 1, 0);
    m_outputPathEdit = new QLineEdit(this);
    m_outputPathEdit->setPlaceholderText("Куда сохранить документацию...");
    paramsLayout->addWidget(m_outputPathEdit, 1, 1);

    auto* btnBrowseOut = new QPushButton("📂", this);
    btnBrowseOut->setMaximumWidth(40);
    paramsLayout->addWidget(btnBrowseOut, 1, 2);

    paramsLayout->addWidget(new QLabel("Формат:"), 2, 0);
    m_formatCombo = new QComboBox(this);
    m_formatCombo->addItems({ "HTML", "Markdown", "AsciiDoc" });
    paramsLayout->addWidget(m_formatCombo, 2, 1);

    mainLayout->addWidget(paramsBox);

    // === Кнопки ===
    auto* btnLayout = new QHBoxLayout();
    m_btnGenerate = new QPushButton("🚀 Сгенерировать", this);
    m_btnCancel = new QPushButton("🛑 Отмена", this);
    m_btnCancel->setEnabled(false);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnGenerate);
    btnLayout->addWidget(m_btnCancel);
    mainLayout->addLayout(btnLayout);

    // === Прогресс ===
    m_progressBar = new QProgressBar(this);
    m_progressBar->setTextVisible(true);
    mainLayout->addWidget(m_progressBar);

    // === Лог ===
    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText("Здесь появится лог генерации...");
    mainLayout->addWidget(m_logEdit);

    // Подключение кнопок browse
    connect(btnBrowseIn, &QPushButton::clicked, this, &MainWindow::onBrowseInput);
    connect(btnBrowseOut, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
}

void MainWindow::setupConnections()
{
    m_worker = new CogentWorker();
    m_worker->moveToThread(m_workerThread);
    m_workerThread->start();

    connect(m_btnGenerate, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(m_btnCancel, &QPushButton::clicked, this, &MainWindow::onCancel);

    connect(m_worker, &CogentWorker::logMessage, this, &MainWindow::onWorkerLog, Qt::QueuedConnection);
    connect(m_worker, &CogentWorker::progressChanged, this, &MainWindow::onWorkerProgress, Qt::QueuedConnection);
    connect(m_worker, &CogentWorker::finished, this, &MainWindow::onWorkerFinished, Qt::QueuedConnection);
}

void MainWindow::onBrowseInput()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Выберите папку с исходниками");
    if (!dir.isEmpty()) m_inputPathEdit->setText(dir);
}

void MainWindow::onBrowseOutput()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Выберите выходную папку");
    if (!dir.isEmpty()) m_outputPathEdit->setText(dir);
}

void MainWindow::onGenerate()
{
    QString in = m_inputPathEdit->text().trimmed();
    QString out = m_outputPathEdit->text().trimmed();
    QString fmt = m_formatCombo->currentText();

    if (in.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите входную папку.");
        return;
    }
    if (out.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите выходную папку.");
        return;
    }

    m_logEdit->clear();
    setUIEnabled(false);
    m_btnGenerate->setEnabled(false);
    m_btnCancel->setEnabled(true);
    m_progressBar->setValue(0);

    emit m_worker->startGeneration(in, out, fmt);
}

void MainWindow::onCancel()
{
    m_worker->cancel();
    m_btnCancel->setEnabled(false);
}

void MainWindow::onWorkerLog(const QString& msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logEdit->append(QString("[%1] %2").arg(timestamp, msg));
}

void MainWindow::onWorkerProgress(int current, int total)
{
    if (total > 0) {
        m_progressBar->setMaximum(total);
        m_progressBar->setValue(current);
    }
}

void MainWindow::onWorkerFinished(bool success, const QString& msg)
{
    setUIEnabled(true);
    m_btnGenerate->setEnabled(true);
    m_btnCancel->setEnabled(false);

    QMessageBox::information(this, success ? "Готово" : "Ошибка", msg);
}

void MainWindow::setUIEnabled(bool enabled)
{
    m_inputPathEdit->setEnabled(enabled);
    m_outputPathEdit->setEnabled(enabled);
    m_formatCombo->setEnabled(enabled);
    m_btnGenerate->setEnabled(enabled);
    // Лог и прогресс остаются доступны
}