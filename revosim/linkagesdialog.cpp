#include "linkagesdialog.h"
#include "ui_linkagesdialog.h"

//RJG - links an image to a variable - edit mode
linkagesDialog::linkagesDialog(linkageClass *linkage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::linkagesDialog)
{
    ui->setupUi(this);
    QString title;
    QTextStream labelOut(&title);
    labelOut << "<b>Modify linkage - ";
    labelOut << linkage->variable;
    labelOut << "</b>";
    ui->topLabel->setText(title);
    ui->label->setText("Please use the options below to edit - or delete - your selected linkage.");
    ui->label->setAlignment(Qt::AlignJustify);
    linkageLocal = linkage;
    files = linkage->imageSequence->returnFileList();
    setUpGUI();

    ui->refreshRate->setValue(linkageLocal->imageSequence->returnChangeRate());
    ui->interpolateImages->setChecked(linkageLocal->interpolate);

    switch (linkageLocal->mode)
    {
    case ENV_MODE_STATIC:
        modeStaticButton->setChecked(true);
        break;
    case ENV_MODE_ONCE:
        modeOnceButton->setChecked(true);
        break;
    case ENV_MODE_LOOP:
        modeLoopButton->setChecked(true);
        break;
    case ENV_MODE_BOUNCE:
        modeBounceButton->setChecked(true);
        break;
    default:
        QMessageBox::warning(this, "Error", "Failed to initialise linkage mode - please email RJG.");
    }

    ui->comboBox->setEnabled(false);
    ui->comboBox->setVisible(false);
    ui->label_2->setVisible(false);
    ui->loadImages->setText("Change images");

    linkageLocal->set = true;

    QPushButton *deleteButton = new QPushButton("&Delete this linkage");
    deleteButton->setObjectName("deleteButton");
    deleteButton->setToolTip("<font>Remove this linkage.</font>");
    ui->verticalLayout_2->addWidget(deleteButton);
    connect(deleteButton, &QPushButton::clicked, this, [ & ]()
    {
        if (QMessageBox::question(this, tr("Delete linkage"), tr("Are you sure you want to delete this linkage?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
        {
            linkageLocal->set = false;
            done(QDialog::Accepted);
        }
    });
    for (auto &f : files) ui->imageListDisplay->append(f);
}

//RJG - links an image to a variable - mode create new, sent a blank from mainwindow to populate, simulationManager to provide combo box list
linkagesDialog::linkagesDialog(linkageClass *linkage, const SimManager *simulationManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::linkagesDialog)
{
    ui->setupUi(this);
    ui->topLabel->setText(QString("<b>Create new linkage</b>"));
    ui->label->setAlignment(Qt::AlignJustify);
    linkageLocal = linkage;
    setUpGUI();

    ui->refreshRate->setValue(100);
    ui->interpolateImages->setChecked(true);
    modeLoopButton->setChecked(true);
    linkageLocal->mode = ENV_MODE_LOOP;

    linkageLocal->set = true;

    //Set linkages
    for (auto &l : simulationManager->linkages) if (!l.set) ui->comboBox->addItem(l.variable);

    linkageLocal->variable = ui->comboBox->currentText();
    connect(ui->comboBox, &QComboBox::currentTextChanged, this,
            [ = ]()
    {
        linkageLocal->variable = ui->comboBox->currentText();
    });
}

void linkagesDialog::setUpGUI()
{
    modeStaticButton = new QRadioButton("Static");
    modeOnceButton = new QRadioButton("Once");
    modeLoopButton = new QRadioButton("Loop");
    modeBounceButton = new QRadioButton("Bounce");

    ui->gridLayout->addWidget(modeStaticButton, 0, 0);
    ui->gridLayout->addWidget(modeOnceButton, 0, 1);
    ui->gridLayout->addWidget(modeLoopButton, 1, 0);
    ui->gridLayout->addWidget(modeBounceButton, 1, 1);

    auto modeButtonGroup = new QButtonGroup;
    modeButtonGroup->addButton(modeStaticButton, ENV_MODE_STATIC);
    modeButtonGroup->addButton(modeOnceButton, ENV_MODE_ONCE);
    modeButtonGroup->addButton(modeLoopButton, ENV_MODE_LOOP);
    modeButtonGroup->addButton(modeBounceButton, ENV_MODE_BOUNCE);
    connect(modeButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [ = ](const int i)
    {
        linkageLocal->mode = i;
    });

    QObject::connect(ui->loadImages, SIGNAL(clicked()), this, SLOT(loadImages()));
    ui->loadImages->setFocusPolicy(Qt::NoFocus);

    connect(ui->interpolateImages, &QCheckBox::stateChanged, this, [ = ](const int i)
    {
        i == 2 ? linkageLocal->interpolate = true : linkageLocal->interpolate = false;
    });

    connect(ui->refreshRate, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [ = ](int i)
    {
        refreshRate = i;
    });
}

void linkagesDialog::loadImages()
{
    files.clear();
    QString desktop = QString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    files = QFileDialog::getOpenFileNames(this, "Select one or more image to control the required variable.", desktop, "Images (*.png *.bmp)");

    if (files.length() == 0) return;
    ui->imageListDisplay->clear();
    for (auto &s : files) ui->imageListDisplay->append(s);
}

//RJG - override done method to check files have been loaded
void linkagesDialog::done(int r)
{
    //RJG - linkage is set to false before this is called
    if (QDialog::Accepted == r && !linkageLocal->set)
    {
        delete linkageLocal->imageSequence;
        linkageLocal->imageSequence = nullptr;
        linkageLocal->mode = 0;
        linkageLocal->interpolate = true;
        QDialog::done(r);
    }
    else if (QDialog::Accepted == r && files.length() == 0) QMessageBox::warning(this, "Error", "Please select some images to control the variable.");
    else
    {
        if (linkageLocal->imageSequence != nullptr)delete linkageLocal->imageSequence;
        linkageLocal->imageSequence = new ImageSequence(files, refreshRate);
        QDialog::done(r);
    }
}

linkagesDialog::~linkagesDialog()
{
    delete modeStaticButton;
    delete modeOnceButton;
    delete modeLoopButton;
    delete modeBounceButton;
    delete ui;
}
