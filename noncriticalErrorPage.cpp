#include "noncriticalErrorPage.h"
#include "ui_noncriticalErrorPage.h"

NoncriticalErrorPage::NoncriticalErrorPage(QWidget *parent) :
    Page(parent),
    ui(new Ui::NoncriticalErrorPage)
{
    ui->setupUi(this);
}

NoncriticalErrorPage::~NoncriticalErrorPage()
{
    delete ui;
}

void NoncriticalErrorPage::init(MainWindow *mainWindow)
{

}

QString NoncriticalErrorPage::getName() const
{
    return "noncriticalError";
}
