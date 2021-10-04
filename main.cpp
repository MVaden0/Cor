#include "cor.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	// initialization of main application class
	QApplication app(argc, argv);

	// initialization of the main window in the application and show maximize on entry
	MainWindow mainWindow;
	mainWindow.showMaximized();

	// executes the application
	return app.exec();
}