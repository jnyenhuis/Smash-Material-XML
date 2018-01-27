#include "Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "Headers/materialxml.h"
#include "Headers/materialcontainer.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Enable/disable the different search options.
    ui->srcContainer->setEnabled(ui->srcCheckBox->isChecked());
    ui->dstContainer->setEnabled(ui->dstCheckBox->isChecked());
    ui->flagsContainer->setEnabled(ui->flagsCheckBox->isChecked());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Xml", ".", "Xml files (*.xml)");
    MaterialXml::addMaterialsFromXML(fileName);

    displayMaterials();
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home",
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
    QDirIterator it(directory, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if (it.next().endsWith(".xml")) {
            QString fileName = it.next();
            MaterialXml::addMaterialsFromXML(fileName);
        }
    }

    displayMaterials();
}

void MainWindow::on_flagsCheckBox_clicked()
{
    bool isChecked = ui->flagsCheckBox->isChecked();
    ui->flagsContainer->setEnabled(isChecked);
    searchSettings->searchFlags = isChecked;
}

void MainWindow::on_dstCheckBox_clicked()
{
    bool isChecked = ui->dstCheckBox->isChecked();
    ui->dstContainer->setEnabled(isChecked);
    searchSettings->searchDst = isChecked;
}

void MainWindow::on_srcCheckBox_clicked()
{
    bool isChecked = ui->srcCheckBox->isChecked();
    ui->srcContainer->setEnabled(isChecked);
    searchSettings->searchSrc = isChecked;
}

void MainWindow::displayMaterials()
{
    // clear the text display for each new search
    ui->plainTextEdit->clear();

    for (int i = 0; i < materialList.length(); i++) {
        Material material = materialList.at(i);

        bool validMaterial = true;
        bool validFlags = true;
        bool validSrc = true;
        bool validDst = true;

        // Check flags using the selected flags values and comparison operator.
        if (searchSettings->searchFlags) {
            int index = ui->flagsOpComboBox->currentIndex();
            SearchSettings::ComparisonOp comparison = (SearchSettings::ComparisonOp) index;

            switch (comparison) {
                case SearchSettings::ComparisonOp::equals:
                    validFlags = (material.getFlags() & searchSettings->getFlags1()) == searchSettings->getFlags2();
                    break;
                case SearchSettings::ComparisonOp::greater:
                    validFlags = (material.getFlags() & searchSettings->getFlags1()) > searchSettings->getFlags2();
                    break;
                case SearchSettings::ComparisonOp::gEqual:
                    validFlags = (material.getFlags() & searchSettings->getFlags1()) >= searchSettings->getFlags2();
                    break;
                case SearchSettings::ComparisonOp::less:
                    validFlags = (material.getFlags() & searchSettings->getFlags1()) < searchSettings->getFlags2();
                    break;
                case SearchSettings::ComparisonOp::lEqual:
                    validFlags = (material.getFlags() & searchSettings->getFlags1()) <= searchSettings->getFlags2();
                    break;
            }
        }

        if (searchSettings->searchSrc) {
            int index = ui->srcOpComboBox->currentIndex();
            SearchSettings::ComparisonOp comparison = (SearchSettings::ComparisonOp) index;
            validSrc = SearchSettings::matchesSearch(comparison, material.srcFactor, searchSettings->srcFactor);
        }

        if (searchSettings->searchDst) {
            int index = ui->dstOpComboBox->currentIndex();
            SearchSettings::ComparisonOp comparison = (SearchSettings::ComparisonOp) index;
            validDst = SearchSettings::matchesSearch(comparison, material.dstFactor, searchSettings->dstFactor);
        }

        validMaterial = validFlags && validSrc && validDst;
        if (validMaterial) {
            ui->plainTextEdit->appendPlainText(material.fileName);

            if (validFlags) {
                QString flags;
                ui->plainTextEdit->appendPlainText(flags.setNum(material.getFlags(), 16));
            }

            if (validSrc) {
                QString src;
                ui->plainTextEdit->appendPlainText("src: " + src.setNum(material.srcFactor, 16));
            }

            if (validDst) {
                QString dst;
                ui->plainTextEdit->appendPlainText("dst: " + dst.setNum(material.dstFactor, 16));
            }

            ui->plainTextEdit->appendPlainText("\n");
        }
    }
}

void MainWindow::on_searchPushButton_clicked()
{
    displayMaterials();
}

void MainWindow::on_clearPushButton_clicked()
{
    ui->plainTextEdit->clear();
}

void MainWindow::on_flags1LineEdit_editingFinished()
{
    // Use hex format. Ex: 9A011063
    QString text = ui->flags1LineEdit->text();
    bool ok;
    searchSettings->setFlags1(text.toUInt(&ok, 16));
}

void MainWindow::on_flags2LineEdit_editingFinished()
{
    // Use hex format. Ex: 9A011063
    QString text = ui->flags2LineEdit->text();
    bool ok;
    searchSettings->setFlags2(text.toUInt(&ok, 16));
}

void MainWindow::on_srcLineEdit_editingFinished()
{
    // Use hex format
    QString text = ui->srcLineEdit->text();
    bool ok;
    searchSettings->srcFactor = (text.toInt(&ok, 16));
}

void MainWindow::on_dstLineEdit_editingFinished()
{
    // Use hex format
    QString text = ui->dstLineEdit->text();
    bool ok;
    searchSettings->dstFactor = (text.toInt(&ok, 16));
}

void MainWindow::on_actionAbout_triggered()
{
    // Display license information.
    QString link = "<a href='https://github.com/ScanMountGoat/Smash-Material-XML/blob/master/license.txt'>GPL License</a>";
    QMessageBox::about(0, "About", link);
}

void MainWindow::on_matPropLineEdit_editingFinished()
{

}

void MainWindow::on_matPropCheckBox_clicked()
{
    bool isChecked = ui->srcCheckBox->isChecked();
    ui->matPropContainer->setEnabled(isCheked);
}