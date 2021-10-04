#include "cor.h"

#include <iostream>
#include <string>

#include <QString>
#include <QThread>
#include <QDragEnterEvent>
#include <QImage>
#include <vector>
#include <QMessageBox>

#include <opencv2/opencv.hpp>


using namespace cv;


bool DragDropFilter::eventFilter(QObject* obj, QEvent* event)
{
	// object casting to QWidget child class
	auto w = qobject_cast<QWidget*>(obj);

	// return true if the widget is null; does nothing
	if (w == nullptr) {
		return true;
	}

	// switch for determining class type of event
	switch (event->type()) {
	case QEvent::DragEnter:
	{
		// casting to QDragEnterEvent for enter event
		auto drag_enter_event = static_cast<QDragEnterEvent*>(event);

		// accepting the event and subsequent action
		drag_enter_event->acceptProposedAction();

		// using QT 'emit' mechanism to emit signal denote file has entered drag space
		emit dragEntered(drag_enter_event->mimeData());
		break;
	}
	case QEvent::DragLeave:
	{
		// casting to QDragLeaveEvent for leave event
		auto drag_leave_event = static_cast<QDragLeaveEvent*>(event);

		// accepting the action
		drag_leave_event->accept();

		// using QT 'emit' mechanism to emit signal to denote file has departed drag space
		emit dragDeparted();
		break;
	}
	case QEvent::Drop:
	{
		// casting to QDropEvent for leave event
		auto drop_event = static_cast<QDropEvent*>(event);

		// acepting the event and subsequent action
		drop_event->acceptProposedAction();

		// using QT 'emit' mechanism to emit signal to denote file has been dropped and data has been collected
		emit dragDropped(drop_event->mimeData());
		break;
	}
	default:

		// returns self for program continuity
		return QObject::eventFilter(obj, event);
	}

	// QT 'eventFilter' objects must return boolean objects
	return true;
}


ClearLineEdit::ClearLineEdit(QWidget* parent) : QLineEdit(parent)
{
	// text setting of child 'QLineEdit' with entry prompt
	this->setText("enter file path or drag image...");

	// geometry and styling
	this->resize((screenWidth / 2) - 50, 30);
	this->move((screenWidth / 2) + 5, 5);
	this->setStyleSheet("QLineEdit {"
		"color: #A3B1C4;"
		"border: 1px solid #7BA7AB;"
		"border-radius: 5px;"
		"font: 8pt Comic Sans MS;"
		"}");

	// drag functionality
	this->setDragEnabled(true);
	this->setAcceptDrops(true);

	// event filter installation
	dragDropFilter = new DragDropFilter();
	this->installEventFilter(dragDropFilter);

	// connecting QT slot for event filter handling
	connect(dragDropFilter,
		&DragDropFilter::dragDropped,
		this,
		[this](const QMimeData* mimeData) {this->setText(this->formatImagePath((mimeData->urls()[0].toString()))); });

	// file path variable
	QString path = this->text();
}
QString ClearLineEdit::formatImagePath(QString currentImagePath)
{
	// getting the position for start of file path
	int imageIndex = currentImagePath.indexOf(":/U");

	// returning the parsed file path as QString
	return currentImagePath.remove(0, imageIndex - 1);
}


SliderSwitch::SliderSwitch(QString name, QWidget* parent) : QPushButton(parent)
{
	// switch state for on/off
	switchState = true;

	// background initialization, geometry, and styling
	background = new QLabel(parent);
	background->resize(150, 30);
	background->setStyleSheet("QLabel {"
		"background-color: #854836;"
		"border-style: solid;"
		"border-radius: 10px;"
		"}");

	// "on" initialization, geometry, and styling
	onLabel = new QLabel("On", parent);
	onLabel->resize(20, 10);
	onLabel->setStyleSheet("QLabel {"
		"background-color: #854836;"
		"color: #e98061;"
		"font: 10pt Comic Sans MS;"
		"}");

	// "off" initialization, geometry, and styling
	offLabel = new QLabel("Off", parent);
	offLabel->resize(30, 10);
	offLabel->setStyleSheet("QLabel {"
		"background-color: #854836;"
		"color: #e98061;"
		"font: 10pt Comic Sans MS;"
		"}");

	// button initialization, geometry, and styling
	this->setText(name);
	this->resize(100, 30);
	this->setStyleSheet("QPushButton {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-radius: 10px;"
		"font: 10pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// connecting QT slot for animation and functionality
	connect(this, &QPushButton::clicked, this, &SliderSwitch::switched);
}
void SliderSwitch::movePos(int x, int y)
{
	// custom movement for widget against native 'move' method
	background->move(x, y);

	// label movement
	onLabel->move(x + 125, y + 10);
	offLabel->move(x + 10, y + 10);

	// moving the button
	this->move(x, y);

	// raising the button to the top of the buttons
	this->raise();
	this->activateWindow();
	this->show();
}
void SliderSwitch::switched() {
	// initializing the sliding animation
	slideAnimation = new QPropertyAnimation(this, "geometry");

	// setting duration and start value
	slideAnimation->setDuration(100);
	slideAnimation->setStartValue(this->geometry());

	// logic for determining direction from switch state
	if (switchState) {
		slideAnimation->setEndValue(QRect(this->x() + 50, this->y(), this->width(), this->height()));
	}
	else {
		slideAnimation->setEndValue(QRect(this->x() - 50, this->y(), this->width(), this->height()));
	}

	// switch state negation
	switchState = !(switchState);

	// setting the animation acceleration
	slideAnimation->setEasingCurve(QEasingCurve::InCubic);

	// start animation
	slideAnimation->start();
}


SpinBox::SpinBox(int max, int min, QWidget* parent)
	: QFrame(parent) {
	// geometry
	this->resize(40, 30);

	// '+' button initialization, geometry, and styling
	plusButton = new QPushButton("+", this);
	plusButton->resize(13, 13);
	plusButton->move(27, 0);
	plusButton->setStyleSheet("QPushButton {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-radius: 3px;"
		"font: 10pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// connecting QT slot for spin box addition
	connect(plusButton, &QPushButton::clicked, this, [this, max]() {plus(max); });

	// '-' button initialization, geometry, and styling
	minusButton = new QPushButton("-", this);
	minusButton->resize(13, 13);
	minusButton->move(27, 17);
	minusButton->setStyleSheet("QPushButton {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-radius: 3px;"
		"font: 10pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// connecting QT slot for spin box subtraction
	connect(minusButton, &QPushButton::clicked, this, [this, min]() {minus(min); });

	// count initialization, geometry, and styling
	countbox = new QLabel("1", this);
	countbox->resize(23, 30);
	countbox->move(0, 0);
	countbox->setStyleSheet("QLabel {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-radius: 3px;"
		"font: 10pt Comic Sans MS;"
		"}");

	// count initialization for k value
	count = countbox->text().toInt();
}
void SpinBox::plus(int maximum)
{
	// logic for maximum count
	if (count < maximum) {
		// incremental iteration and count update
		count += 1;
		countbox->setText(QString::number(count));
	}
}
void SpinBox::minus(int minimum)
{
	// logic for minimum count
	if (count > minimum) {
		// decremental iteration and count update
		count -= 1;
		countbox->setText(QString::number(count));
	}
}


ColorDialog::ColorDialog(QWidget* parent)
	: QDialog(parent) {
	this->setWindowTitle("color reassignment");

	entryButton = new QPushButton("Enter", this);
	entryButton->resize(100, 30);
	entryButton->setStyleSheet("QPushButton {"
		"color: #A3B1C4;"
		"border: 1px solid #7BA7AB;"
		"border-style: solid;"
		"border-radius: 5px;"
		"font: 8pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	colorPaletteDialog = new QColorDialog(this);
}
void ColorDialog::updateColors()
{
	for (int i = 0; i < colors.size(); i++) {
		QString r = QString::number(colors[i][0]);
		QString g = QString::number(colors[i][1]);
		QString b = QString::number(colors[i][2]);

		QString rDark = QString::number((int)colors[i][0] * 0.8);
		QString gDark = QString::number((int)colors[i][1] * 0.8);
		QString bDark = QString::number((int)colors[i][2] * 0.8);

		if (i >= colorLabelArray.size()) {
			colorLabelArray.append(new ColorDialogRow(this));
			colorLabelArray[i]->move(65, 10 + (30 * i));
			colorLabelArray[i]->precolor->setStyleSheet(QString("background:rgb(%1, %2, %3)").arg(r, g, b));
			colorLabelArray[i]->postcolor->setStyleSheet(QString("QPushButton {"
				"background:rgb(%1, %2, %3);"
				"border: none;"
				"} QPushButton::hover {"
				"background:rgb(%4, %5, %6)"
				"}").arg(r, g, b, rDark, gDark, bDark));

			connect(colorLabelArray[i]->postcolor, &QPushButton::released, this,
				[this, i]() {
					colorPaletteDialog->show(); chosenColorIndex = i;
				});

			connect(colorPaletteDialog, &QColorDialog::colorSelected, this,
				[this]() {
					QString color = QColor(colorPaletteDialog->selectedColor()).name();

					colorLabelArray[chosenColorIndex]->postcolor->setStyleSheet(QString("QPushButton {"
						"background: %1;"
						"border: none;"
						"} QPushButton::hover {"
						"background:rgb(%4, %5, %6)"
						"}").arg(color));
				});
		}
	}

	this->resize(300, 50 + colors.size() * 30);
	entryButton->move(100, 10 + colors.size() * 30);
}


ColorDialogRow::ColorDialogRow(QWidget* parent)
	: QFrame(parent) {
	this->resize(170, 20);

	precolor = new QLabel(this);
	precolor->resize(20, 20);
	precolor->move(0, 0);

	postcolor = new QPushButton(this);
	postcolor->resize(20, 20);
	postcolor->move(150, 0);

	arrow = new QLabel("-->", this);
	arrow->setAlignment(Qt::AlignCenter);
	arrow->resize(30, 20);
	arrow->move(70, 0);
	arrow->setStyleSheet("color: #A3B1C4;");

	int r = qRed(this->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
	int g = qGreen(this->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
	int b = qBlue(this->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
}


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent) {
	// menu page creation
	menuPage = new MenuPage(this);

	// screen resize to size of monitor
	screenWidth = QGuiApplication::primaryScreen()->geometry().width();
	screenHeight = QGuiApplication::primaryScreen()->geometry().height();
	this->resize(MainWindow::screenWidth, MainWindow::screenHeight);
}


MainPage::MainPage(QWidget* parent)
	: QFrame(parent) {
	// initializing width and height of screen
	screenWidth = QGuiApplication::primaryScreen()->geometry().width();
	screenHeight = QGuiApplication::primaryScreen()->geometry().height();

	// geometry and styling
	this->resize(screenWidth, screenHeight);
	this->move(0, 0);
	this->setStyleSheet("background-color: #285A5E");

	// line edit initialization
	fileInput = new ClearLineEdit(this);

	// accepts drops in screen for drag and drop functionality
	this->setAcceptDrops(true);

	// picture frame creation, geometry, and styling
	pictureFrame = new QFrame(this);
	pictureFrame->resize((screenWidth / 2) - 10, screenHeight - 35);
	pictureFrame->move(5, 5);
	pictureFrame->setStyleSheet("QFrame {"
		"border: 1px solid #7BA7AB;"
		"border-radius: 5px;"
		"}");

	// entry button creation, geometry, and styling
	loadButton = new QPushButton("Load", this);
	loadButton->resize(35, 30);
	loadButton->move(((screenWidth / 2) + 5) + ((screenWidth / 2) - 50) + 5, 5);
	loadButton->setStyleSheet("QPushButton {"
		"color: #A3B1C4;"
		"border: 1px solid #7BA7AB;"
		"border-style: solid;"
		"border-radius: 5px;"
		"font: 8pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// slot connection for raw file input
	connect(loadButton,
		&QPushButton::released,
		this,
		[this]() {updateImagePath(false, false, false, 0, 0, false); });

	// label initialization for frame that picture is contained in
	pic = new QLabel(pictureFrame);

	// manipulation block
	bounding = new QLabel(this);
	bounding->resize((screenWidth / 2) - 10, 112);
	bounding->move((screenWidth / 2) + 5, 45);
	bounding->setStyleSheet("QLabel {"
		"background-color: transparent;"
		"border: 1px solid #7BA7AB;"
		"border-radius: 5px;"
		"}");

	// switch creation and geometry
	pixelation = new SliderSwitch("PIXELATION", this);
	pixelation->movePos((screenWidth / 2) + 10, 50);

	kMeans = new SliderSwitch("K-MEANS", this);
	kMeans->movePos((screenWidth / 2) + 10, 85);

	hues = new SliderSwitch("HUES", this);
	hues->movePos((screenWidth / 2) + 10, 120);

	// k value component creation, geometry, and styling
	kValue = new SpinBox(10, 1, this);
	kValue->move((screenWidth / 2) + 185, 85);

	kLabel = new QLabel("K:", this);
	kLabel->move((screenWidth / 2) + 167, 90);
	kLabel->setStyleSheet("QLabel {"
		"color: #e98061;"
		"font: 10pt Comic Sans MS;"
		"}");

	// pixel factor component creation, geometry, and styling
	pixFactor = new SpinBox(128, 1, this);
	pixFactor->move((screenWidth / 2) + 185, 50);

	pixLabel = new QLabel("P:", this);
	pixLabel->move((screenWidth / 2) + 167, 55);
	pixLabel->setStyleSheet("QLabel {"
		"color: #e98061;"
		"font: 10pt Comic Sans MS;"
		"}");

	pixStretch = new SliderSwitch("Stretch", this);
	pixStretch->movePos(1000, 50);

	// entry button creation, geometry, and styling
	entryButton = new QPushButton("Enter", this);
	entryButton->resize(100, 30);
	entryButton->move(((screenWidth / 4) * 3) - 50, 165);
	entryButton->setStyleSheet("QPushButton {"
		"color: #A3B1C4;"
		"border: 1px solid #7BA7AB;"
		"border-style: solid;"
		"border-radius: 5px;"
		"font: 8pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// slot connection for k means image
	connect(entryButton,
		&QPushButton::released,
		this,
		[this]() {updateImagePath(!(kMeans->switchState),
			!(pixelation->switchState),
			!(hues->switchState),
			kValue->count,
			pixFactor->count,
			!(pixStretch->switchState)); });

	// colors and colorDialog initialization, geometry, and styling
	colors = new QPushButton("Colors", this);
	colors->move((screenWidth / 2) + 168, 120);
	colors->resize(58, 30);
	colors->setStyleSheet("QPushButton {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-radius: 10px;"
		"font: 10pt Comic Sans MS;"
		"}");

	colorDialog = new ColorDialog(this);

	// button connection for displaying color dialog
	connect(colors,
		&QPushButton::released,
		this,
		[this]() {colorDialog->show(); });

}

Mat MainPage::matFormat(String image_path)
{
	// initialization and output of formatted 'rgb' image matrix from filepath
	Mat input = imread(image_path);
	Mat output;

	cvtColor(input, output, COLOR_BGR2RGB);

	return output;
}
void MainPage::updateImagePath(bool kmeans, bool pixelate, bool hues, int k, int pixFactor, bool pixStretch)
{
	// initializing the pixmap from the file text
	Mat image = matFormat((fileInput->text()).toStdString());

	// switch logic
	if (kmeans) {
		// k means algorithm
		image = kMeansImage(image, k);
	}
	if (pixelate) {
		// pixealtion interpolation
		image = pixelateImage(image, pixFactor, pixStretch);
	}
	if (hues) {
		image = huesImage(image, colorDialog);
	}

	// initialization of pixmap from image
	QPixmap picPixmap = pixmapFormat(image);

	// setting the pixmap and parent of the 'pic' label
	pic->setPixmap(picPixmap.scaled(pictureFrame->width(), pictureFrame->height(), Qt::KeepAspectRatio));

	pic->setAlignment(Qt::AlignCenter);
	pic->setParent(pictureFrame);
	pic->resize(pictureFrame->width(), pictureFrame->height());

	// image output
	pic->show();
	pic->activateWindow();
	pic->raise();
}
Mat MainPage::kMeansImage(Mat image, int k)
{
	// matrix initialization for image data
	Mat data;

	// matrix conversion and reshaping
	image.convertTo(data, CV_32F);
	data = data.reshape(1, data.total());

	// kmeans algorithm execution
	Mat labels, centers;
	TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 10, 1.0);
	kmeans(data, k, labels, criteria, 3,
		KMEANS_PP_CENTERS, centers);

	// reshaping to tensors with size 3 for color channels
	centers = centers.reshape(3, centers.rows);
	data = data.reshape(3, data.rows);

	// replace pixel value with center value
	Vec3f* p = data.ptr<Vec3f>();
	for (size_t i = 0; i < data.rows; i++) {
		int center_id = labels.at<int>(i);
		p[i] = centers.at<Vec3f>(center_id);
	}

	// reshaping to 2d matirx for image output
	image = data.reshape(3, image.rows);
	image.convertTo(image, CV_8U);

	// gathers the colors of the clusters produced by the kmeans algorithm
	gatherColors(image);
	//colors->addWidgets();

	// return of processed image
	return image;
}
Mat MainPage::pixelateImage(Mat image, int factor, bool stretch)
{
	// initializing intermediate and output matrices 
	Mat intermediate;
	Mat output;

	// resizing image to size determined by factor, using OpenCV flag INTER_AREA to save image details
	cv::resize(image, intermediate, Size(factor, factor), 0, 0, INTER_AREA);

	// logic whether or not to stretch output image to original aspect ratio or square
	if (stretch) {
		cv::resize(intermediate, output, Size(image.cols, image.cols), 0, 0, INTER_NEAREST);
	}
	else {
		cv::resize(intermediate, output, Size(image.cols, image.rows), 0, 0, INTER_NEAREST);
	}

	return output;
}
Mat MainPage::huesImage(Mat image, ColorDialog* dialog)
{
	// initializing arrays for colorLabelObjects and the initial and filtered colors within them
	QVector<ColorDialogRow*> arr = dialog->colorLabelArray;
	std::vector<Vec3b> colors = dialog->colors;
	std::vector<Vec3b> processedColors = dialog->processedColors;

	// iteration over
	int size = arr.size();

	for (int i = 0; i < size; i++) {
		// setting the (r, g, b) values from the initial colors contained in the colorDialog, given by the kmeans algorithm
		int red = qRed(arr[i]->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int green = qGreen(arr[i]->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int blue = qBlue(arr[i]->precolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());

		// assignment to c
		Vec3b c;
		c[0] = red;
		c[1] = green;
		c[2] = blue;
		colors.push_back(c);

		// setting the (r, g, b) values from the processed colors contained in the colorDialog, given by the kmeans algorithm
		int pRed = qRed(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int pGreen = qGreen(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int pBlue = qBlue(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());

		// assignment to pC
		Vec3b pC;
		pC[0] = pRed;
		pC[1] = pGreen;
		pC[2] = pBlue;
		processedColors.push_back(pC);

	}

	// iteration over image to check if color of pixel is equal to any colors in the initial color
	// then, this pixel will be set to the filtered color chosen
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			Vec3b key = image.at<Vec3b>(i, j);

			std::vector<Vec3b>::iterator iterator = std::find(colors.begin(), colors.end(), key);

			int index;

			if (iterator != colors.end()) {
				index = std::distance(colors.begin(), iterator);

			}

			image.at<Vec3b>(i, j) = processedColors[index];

		}
	}

	// iteration over colors to set initial colors to filtered colors after execution of algorithm
	for (int i = 0; i < processedColors.size(); i++) {
		int r = qRed(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int g = qGreen(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());
		int b = qBlue(arr[i]->postcolor->palette().color(QWidget::backgroundRole()).toRgb().rgb());

		Vec3b preColor;
		preColor[0] = r;
		preColor[1] = g;
		preColor[2] = b;

		QColor color(arr[i]->postcolor->palette().color(QWidget::backgroundRole()));

		QString style = QString("background-color: rgb(%1, %2, %3);").arg(QString::number(color.red()),
			QString::number(color.green()),
			QString::number(color.blue()));

		arr[i]->precolor->setStyleSheet(style);
	}

	return image;
}
void MainPage::gatherColors(Mat image) {
	// deinitializes the available color list
	availableColors.clear();

	// iterates over whole image to compartmentalize every color
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			if (availableColors.size() == 0) {
				availableColors.push_back(image.at<Vec3b>(i, j));
			}
			else {
				for (int k = 0; k < availableColors.size(); k++) {
					if (std::find(availableColors.begin(), availableColors.end(), image.at<Vec3b>(i, j)) != availableColors.end()) {
						break;
					}
					else {
						availableColors.push_back(image.at<Vec3b>(i, j));
					}
				}
			}
		}
	}

	// updating the color arrays
	colorDialog->colors = availableColors;
	colorDialog->updateColors();
}
QPixmap MainPage::pixmapFormat(Mat image) {
	// outputs a pixmap from a Mat image
	QPixmap pixmap = QPixmap::fromImage(QImage((unsigned char*)image.data, image.cols, image.rows, QImage::Format_RGB888));
	return pixmap;
}

MenuPage::MenuPage(QWidget* parent)
	: QFrame(parent) {
	// screen dimensions must be newly initialized in each class
	screenWidth = QGuiApplication::primaryScreen()->geometry().width();
	screenHeight = QGuiApplication::primaryScreen()->geometry().height();

	// main page initialization, geometry, and styling
	mainPage = new MainPage();
	this->resize(screenWidth, screenHeight);
	this->setStyleSheet(QString::fromStdString("background-color: #a0dadf;"));

	// new button initialization, geometry, and styling
	newButton = new QPushButton("new", this);
	newButton->resize(175, 50);
	newButton->move((screenWidth / 2) - int(175 / 2), 500);
	newButton->setStyleSheet("QPushButton {"
		"background-color: #e98061;"
		"color: #214e81;"
		"border-style: solid;"
		"border-radius: 10px;"
		"font: 20pt Comic Sans MS;"
		"} QPushButton::Hover {"
		"background-color: #775B59;"
		"color: #A8F9FF;"
		"}");

	// slot connection for logo animation
	connect(newButton, &QPushButton::released, this, &MenuPage::logoAnimate);

	// offset for 
	int logoOffsetCorrection = 6;

	// label for logo gif creation, geometry, and styling
	logoAnimation = new QLabel(this);
	logoAnimation->resize(500, 200);
	logoAnimation->move((screenWidth / 2) - int(500 / 2) - logoOffsetCorrection, 300);
	logoAnimation->setScaledContents(true);

	// setting label with gif
	logoGif = new QMovie("C:\\Users\\micha\\OneDrive\\Desktop\\plexi\\cor_logo_animation.gif");
	logoAnimation->setMovie(logoGif);

	// label for stationary logo creation, geometry, and styling
	logo = new QLabel(this);
	logo->resize(500, 200);
	logo->move((screenWidth / 2) - int(500 / 2) - logoOffsetCorrection, 300);
	logo->setScaledContents(true);

	logo->setPixmap(QPixmap("C:\\Users\\micha\\OneDrive\\Desktop\\plexi\\cor_logo_start.png"));

	// bounding rectangle for logo animation
	logoBoundingRectangle = new QLabel("", this);
	logoBoundingRectangle->resize(500, 200);
	logoBoundingRectangle->move((screenWidth / 2) - int(500 / 2) - logoOffsetCorrection, 300);
	logoBoundingRectangle->setStyleSheet("QLabel {"
		"background-color: transparent;"
		"border: 4px solid #a0dadf;"
		"}");
}
void MenuPage::logoAnimate()
{
	if (logo != NULL) {
		// raising the logo gif and the bounding rectangle to the front
		logoAnimation->show();
		logoAnimation->activateWindow();
		logoAnimation->raise();

		logoBoundingRectangle->show();
		logoBoundingRectangle->activateWindow();
		logoBoundingRectangle->raise();

		// starting the animation
		logoGif->start();

		// slot connection
		connect(logoGif, &QMovie::finished, this, &MenuPage::screenTransition);
	}
}
void MenuPage::screenTransition()
{
	// wait time for transition animation
	QThread::sleep(0.6);

	// parent setting and raising 
	mainPage->setParent(this);
	mainPage->show();
	mainPage->activateWindow();
	mainPage->raise();

	// label initialization, geometry, styling, raising, and pixmap setting of screen image
	screenGrab = new QLabel(this);
	screenGrab->resize(screenWidth, screenHeight);
	screenGrab->move(0, 0);
	screenGrab->setScaledContents(true);

	screenGrab->show();
	screenGrab->activateWindow();
	screenGrab->raise();

	screenGrabPixmap = QPixmap("C:\\Users\\micha\\OneDrive\\Desktop\\plexi\\screenshot_transition.png");
	screenGrab->setPixmap(screenGrabPixmap);

	// animation property initialization and setting
	transitionAnimation = new QPropertyAnimation(screenGrab, "geometry");
	transitionAnimation->setDuration(1000);
	transitionAnimation->setStartValue(screenGrab->geometry());
	transitionAnimation->setEndValue(QRect(0, screenHeight, screenWidth, screenHeight));
	transitionAnimation->setEasingCurve(QEasingCurve::InCubic);
	transitionAnimation->start();
}
