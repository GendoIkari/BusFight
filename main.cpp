#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication::setDesktopSettingsAware(false);

    QApplication app(argc, argv);
    app.setApplicationName("Bus Fight");

    MainWindow window;
    window.showMaximized();
    return app.exec();
}
