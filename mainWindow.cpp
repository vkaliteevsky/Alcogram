#include <QFontDatabase>
#include <QDir>

#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _configManager(new ConfigManager(this))
{
    _ui->setupUi(this);

    _configManager->load(":/config.conf");

    loadFonts();

    //init pages
    for (int i = 0; i < _ui->pages->count(); i++) {
        Page* page = static_cast<Page*>(_ui->pages->widget(i));

        page->init(this);
    }
}

void MainWindow::setPage(PageName pageName)
{
    _ui->pages->setCurrentIndex(pageName);
}

ConfigManager* MainWindow::getConfigManager() const
{
    return _configManager;
}

void MainWindow::loadFonts()
{
    QDir directory(":/fonts/");
    QStringList fontList = directory.entryList(QStringList("*.ttf"));
    QString ff;

    for (QString font : fontList) {
        int id = QFontDatabase::addApplicationFont(":/fonts/" + font);
        _fonts.append(id);
        ff = QFontDatabase::applicationFontFamilies(id).at(0);
    }
}

void MainWindow::removeFonts()
{
    for (int id : _fonts) {
        QFontDatabase::removeApplicationFont(id);
    }
}

MainWindow::~MainWindow()
{
    delete _ui;

    removeFonts();
}
