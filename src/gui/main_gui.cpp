#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("COGENT");

    QMainWindow window;
    window.setWindowTitle("Test");
    window.resize(450, 250);

    QWidget* central = new QWidget(&window);
    window.setCentralWidget(central);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QLabel* label = new QLabel("Qt6", central);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QPushButton* btn = new QPushButton("Click Me", central);
    layout->addWidget(btn);
    layout->addStretch();

    QObject::connect(btn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "Success", "s");
        });

    window.show();
    return QApplication::exec();
}