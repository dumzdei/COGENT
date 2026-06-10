#include <QApplication>
#include "gui/MainWindow.h" 

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("COGENT");
    app.setOrganizationName("COGENT Team");

    MainWindow window;
    window.show();

    return app.exec();
}