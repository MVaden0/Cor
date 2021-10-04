#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFrame>
#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QPixmap>
#include <QMovie>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QMimeData>
#include <QlineEdit>
#include <QSpinBox>
#include <QToolButton>
#include <QCheckBox>
#include <vector>
#include <QVector>
#include <QList>
#include <QDialog>
#include <QColorDialog>

#include <opencv2/opencv.hpp>

using namespace cv;

// utility classes
class DragDropFilter : public QObject
{
	Q_OBJECT
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

signals:
	void dragEntered(const QMimeData* mimeData);
	void dragDropped(const QMimeData* mimeData);
	void dragDeparted();

};

class ClearLineEdit : public QLineEdit {
	Q_OBJECT

public:
	explicit ClearLineEdit(QWidget* parent = nullptr);

	int screenHeight = QGuiApplication::primaryScreen()->geometry().height();
	int screenWidth = QGuiApplication::primaryScreen()->geometry().width();

	DragDropFilter* dragDropFilter;

	QString formatImagePath(QString currentimagePath);
};

class SliderSwitch : public QPushButton {
	Q_OBJECT

public:
	explicit SliderSwitch(QString name, QWidget* parent = nullptr);

	QLabel* background;
	QPropertyAnimation* slideAnimation;
	QLabel* onLabel;
	QLabel* offLabel;

	bool switchState;

	void movePos(int x, int y);
	void switched();
};

class SpinBox : public QFrame {
	Q_OBJECT

public:
	explicit SpinBox(int max, int min, QWidget* parent = nullptr);
	int max = max;
	int min = min;

	QPushButton* plusButton;
	QPushButton* minusButton;

	QLabel* countbox;

	int count;

private slots:
	void plus(int maximum);
	void minus(int minimum);
};

class ColorDialogRow : public QFrame {
	Q_OBJECT

public:
	explicit ColorDialogRow(QWidget* parent = nullptr);

	QLabel* precolor;
	QPushButton* postcolor;
	QLabel* arrow;


};

class ColorDialog : public QDialog {
	Q_OBJECT

public:
	explicit ColorDialog(QWidget* parent = nullptr);

	void updateColors();
	QVector<ColorDialogRow*> colorLabelArray;

	std::vector<Vec3b> colors;
	std::vector<Vec3b> processedColors;

	QLabel* color;

	QPushButton* entryButton;

	QColorDialog* colorPaletteDialog;

	int chosenColorIndex;

};

class MainPage : public QFrame
{
	Q_OBJECT
public:
	explicit MainPage(QWidget* parent = nullptr);

	int screenHeight;
	int screenWidth;

	QFrame* pictureFrame;
	ClearLineEdit* fileInput;
	QLineEdit* file;
	QPushButton* loadButton;

	QString* filePath;
	QLabel* pic;

	QLabel* bounding;

	SliderSwitch* pixelation;
	SliderSwitch* kMeans;
	SliderSwitch* hues;

	QPushButton* entryButton;

	SpinBox* kValue;
	QLabel* kLabel;

	SpinBox* pixFactor;
	QLabel* pixLabel;
	SliderSwitch* pixStretch;

	QPushButton* colors;
	ColorDialog* colorDialog;
	//DropdownColorDialog* test1;

	std::vector<Vec3b> availableColors;

	QLabel* tester;


private slots:
	void updateImagePath(bool kmeans, bool pixelate, bool hues, int k, int pixFactor, bool pixStretch);

	Mat matFormat(String imagePath);

	Mat kMeansImage(Mat image, int k);
	Mat pixelateImage(Mat image, int factor, bool stretch);
	Mat huesImage(Mat image, ColorDialog* dialog);

	void gatherColors(Mat image);

	//void buttonInit(DropDownColors* buttons);

	QPixmap pixmapFormat(Mat image);
};

class MenuPage : public QFrame
{
	Q_OBJECT
public:
	explicit MenuPage(QWidget* parent = nullptr);
	int screenWidth;
	int screenHeight;

	MainPage* mainPage;

	QPushButton* newButton;
	QLabel* logo;
	QLabel* logoAnimation;
	QMovie* logoGif;
	QLabel* logoBoundingRectangle;

	QPropertyAnimation* transitionAnimation;
	QEasingCurve* acceleration;

	QLabel* screenGrab;
	QPixmap screenGrabPixmap;

private slots:
	void logoAnimate();
	void screenTransition();

};

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);

	int screenWidth;
	int screenHeight;

	static QString lightBlue;

	MenuPage* menuPage;
	MainPage* mainPage;
};

#endif MAINWINDOW_H