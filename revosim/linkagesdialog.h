#ifndef LINKAGESDIALOG_H
#define LINKAGESDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QStandardPaths>
#include "simmanager.h"
#include "linkageclass.h"

namespace Ui {
class linkagesDialog;
}

class linkagesDialog : public QDialog
{
    Q_OBJECT

public:
    //Edit a linkage
    explicit linkagesDialog(linkageClass *linkage, QWidget *parent = nullptr);
    //Create a new linkage - need to send it one from slot to modify, hence linkage here
    explicit linkagesDialog(linkageClass *linkage, const SimManager *simulationManager, QWidget *parent = nullptr);
    ~linkagesDialog();
    Ui::linkagesDialog *ui;

private:
    QRadioButton *modeStaticButton;
    QRadioButton *modeOnceButton;
    QRadioButton *modeLoopButton;
    QRadioButton *modeBounceButton;
    linkageClass *linkageLocal;
    bool deleteLinkage = false;
    void setUpGUI();
    void done(int r);
    QStringList files;
    int refreshRate;

private slots:
    void loadImages();
};

#endif // LINKAGESDIALOG_H
