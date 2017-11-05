#include <QFontDatabase>
#include <QDir>

#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    loadFonts();

    //init pages
    for (int i = 0; i < _ui->stackedWidget->count(); i++) {
        Page* page = static_cast<Page*>(_ui->stackedWidget->widget(i));

        page->init(this);
    }
}

QString MainWindow::getFontFamily() {
    return _fontFamily;
}

void MainWindow::nextPage() {
    int currentIndex = _ui->stackedWidget->currentIndex();
    _ui->stackedWidget->setCurrentIndex(currentIndex + 1);
}

void MainWindow::loadFonts()
{
    QDir directory(":/fonts/");
    QStringList fontList = directory.entryList(QStringList("*.ttf"));

    for (QString font : fontList) {
        int id = QFontDatabase::addApplicationFont(":/fonts/" + font);
        _fonts.append(id);
    }

    _fontFamily = QFontDatabase::applicationFontFamilies(_fonts.at(0)).at(0);
}

void MainWindow::removeFonts() {
    for (int id : _fonts) {
        QFontDatabase::removeApplicationFont(id);
    }
}

MainWindow::~MainWindow()
{
    delete _ui;

    removeFonts();
}
