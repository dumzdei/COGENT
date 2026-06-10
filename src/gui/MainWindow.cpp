#include "gui/MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("COGENT HDL Documentation Generator"));
    resize(800, 600);

    // 1. Строка инструментов (Меню "Файл")
    QMenu* fileMenu = menuBar()->addMenu(tr("Файл"));
    actionCreate = new QAction(tr("Создать"), this);
    actionCreate->setShortcut(QKeySequence::New);
    connect(actionCreate, &QAction::triggered, this, &MainWindow::onActionCreate);
    fileMenu->addAction(actionCreate);

    actionSaveAs = new QAction(tr("Сохранить как..."), this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::onActionSaveAs);
    fileMenu->addAction(actionSaveAs);

    // 2. Центральные виджеты
    editor = new QPlainTextEdit(this);
    editor->setPlaceholderText(tr("Поле для редактирования HDL кода..."));
    editor->setFont(QFont("Consolas", 10));

    // Элементы управления (добавлены компактно, чтобы бэкенд мог работать)
    pathEdit = new QLineEdit(this);
    pathEdit->setPlaceholderText(tr("Папка с HDL файлами для обработки..."));
    pathEdit->setReadOnly(true);

    browseBtn = new QPushButton(tr("Обзор..."), this);

    formatCombo = new QComboBox(this);
    // Замените OutputFormat::html на реальные значения из вашего enum
    formatCombo->addItem(tr("HTML"), static_cast<int>(OutputFormat::html));
    formatCombo->addItem(tr("AsciiDoc"), static_cast<int>(OutputFormat::asciidoc));

    runBtn = new QPushButton(tr("Сгенерировать"), this);
    runBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 6px;");

    logEdit = new QTextEdit(this);
    logEdit->setReadOnly(true);
    logEdit->setFont(QFont("Consolas", 9));
    logEdit->setPlaceholderText(tr("Лог выполнения..."));
    logEdit->setMaximumHeight(150); // Ограничиваем высоту лога, чтобы редактор был большим

    // 3. Компоновка (Layouts)
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Поле редактирования занимает всё свободное пространство (stretch = 1)
    mainLayout->addWidget(editor, 1);

    // Строка управления: Путь + Обзор + Формат + Генерация
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(pathEdit, 1); // Поле пути растягивается
    controlLayout->addWidget(browseBtn);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(new QLabel(tr("Формат:"), this));
    controlLayout->addWidget(formatCombo);
    controlLayout->addWidget(runBtn);

    mainLayout->addLayout(controlLayout);

    // Лог внизу
    mainLayout->addWidget(new QLabel(tr("Лог выполнения:"), this));
    mainLayout->addWidget(logEdit);

    setCentralWidget(centralWidget);

    // 4. Инициализация Worker & Thread (БЕЗОПАСНЫЙ ЖИЗНЕННЫЙ ЦИКЛ)
    workerThread = new QThread(this);
    worker = new CogentWorker();
    worker->moveToThread(workerThread);

    // Используем QueuedConnection для безопасной передачи данных из потока в GUI
    connect(worker, &CogentWorker::logMessage, this, &MainWindow::onWorkerLog, Qt::QueuedConnection);
    connect(worker, &CogentWorker::finished, this, &MainWindow::onWorkerFinished, Qt::QueuedConnection);

    workerThread->start(); // Запускаем поток ОДИН РАЗ при старте окна

    // 5. Сигналы UI
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseClicked);
    connect(runBtn, &QPushButton::clicked, this, &MainWindow::onRunClicked);
}

MainWindow::~MainWindow() {
    // Корректное завершение потока при закрытии программы
    workerThread->quit();
    workerThread->wait();
}

void MainWindow::onActionCreate() {
    editor->clear();
    pathEdit->clear();
    logEdit->clear();
}

void MainWindow::onActionSaveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), "",
        tr("HDL Files (*.v *.sv *.vhd *.vhdl);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << editor->toPlainText();
            file.close();
            logEdit->append(tr("УСПЕХ: Файл сохранен как '%1'").arg(fileName));
        }
        else {
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось сохранить файл."));
        }
    }
}

void MainWindow::onBrowseClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выберите папку с HDL файлами"));
    if (!dir.isEmpty()) {
        pathEdit->setText(dir);
    }
}

void MainWindow::onRunClicked() {
    QString pathStr = pathEdit->text();
    if (pathStr.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Сначала выберите папку с файлами через 'Обзор'!"));
        return;
    }

    logEdit->clear();
    runBtn->setEnabled(false); // Блокируем кнопку на время выполнения

    int formatInt = formatCombo->currentData().toInt();

    // Асинхронный запуск обработки в отдельном потоке
    QMetaObject::invokeMethod(worker, "process", Qt::QueuedConnection,
        Q_ARG(QString, pathStr), Q_ARG(int, formatInt));
}

void MainWindow::onWorkerLog(const QString& message) {
    logEdit->append(message);
}

void MainWindow::onWorkerFinished(bool success, const QString& resultMessage) {
    runBtn->setEnabled(true); // Разблокируем кнопку

    if (success) {
        QMessageBox::information(this, tr("Успех"), resultMessage);
    }
    else {
        QMessageBox::critical(this, tr("Ошибка"), resultMessage);
    }
}