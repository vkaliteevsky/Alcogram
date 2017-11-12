#pragma once

#include <QWidget>

#include "page.h"

namespace Ui {
    class StartPageWidget;
}


class StartPageWidget : public Page
{
    Q_OBJECT

public:
    explicit StartPageWidget(QWidget *parent = 0);
    ~StartPageWidget();

    void init(MainWindow* mainWindow) override;
    QString getName() const override;

private slots:
    void onStartButtonRelease();
    void onSwitchLanguageButtonRelease();

private:
    void initInterface();
    void setConnections();

    Ui::StartPageWidget* _ui;
};
