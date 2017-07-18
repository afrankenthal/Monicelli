/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ================================================================================*/
 
#include "GeometryParameters.h"
#include "ui_geometryParameters.h"
#include "Detector.h"

//===========================================================================
GeometryParameters::GeometryParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometryParameters),
    badValue_(false)
{
    ui->setupUi(this);

    this->setGeometry(0, 0, 960, 45);

    total_.push_back(ui->totalPar0LE);
    total_.push_back(ui->totalPar1LE);
    total_.push_back(ui->totalPar2LE);
    total_.push_back(ui->totalPar3LE);
    total_.push_back(ui->totalPar4LE);
    total_.push_back(ui->totalPar5LE);

    base_.push_back(ui->basePar0LE);
    base_.push_back(ui->basePar1LE);
    base_.push_back(ui->basePar2LE);
    base_.push_back(ui->basePar3LE);
    base_.push_back(ui->basePar4LE);
    base_.push_back(ui->basePar5LE);

    correction_.push_back(ui->correctionPar0LE);
    correction_.push_back(ui->correctionPar1LE);
    correction_.push_back(ui->correctionPar2LE);
    correction_.push_back(ui->correctionPar3LE);
    correction_.push_back(ui->correctionPar4LE);
    correction_.push_back(ui->correctionPar5LE);

    connect(ui->detectorNameCB, SIGNAL(toggled(bool)), ui->parametersGB, SLOT(setEnabled(bool)));

    for(int p=0; p<nParameters; p++)
    {
        connect(base_[p]      , SIGNAL(textChanged(QString)), this, SLOT(checkValue(QString)));
        connect(correction_[p], SIGNAL(textChanged(QString)), this, SLOT(checkValue(QString)));
        connect(total_[p]     , SIGNAL(textChanged(QString)), this, SLOT(checkValue(QString)));
        connect(base_[p]      , SIGNAL(editingFinished()), this, SLOT(updateTotal()));
        connect(correction_[p], SIGNAL(editingFinished()), this, SLOT(updateTotal()));
    }
}

//===========================================================================
GeometryParameters::~GeometryParameters()
{
    delete ui;
}

//===========================================================================
void GeometryParameters::showDetectorPars(Detector* detector)
{
    ui->detectorNameCB->setText(QString::fromStdString(detector->getID()));
    ui->detectorNameCB->setChecked(true);
    ui->basePar0LE      ->setText(QString::number(detector->getXRotation          ()     ,'f',4));
    ui->basePar1LE      ->setText(QString::number(detector->getYRotation          ()     ,'f',4));
    ui->basePar2LE      ->setText(QString::number(detector->getZRotation          ()     ,'f',4));
    ui->basePar3LE      ->setText(QString::number(detector->getXPosition          ()*10  ,'f',4));
    ui->basePar4LE      ->setText(QString::number(detector->getYPosition          ()*10  ,'f',4));
    ui->basePar5LE      ->setText(QString::number(detector->getZPosition          ()/1000,'f',4));
    ui->correctionPar0LE->setText(QString::number(detector->getXRotationCorrection()     ,'f',4));
    ui->correctionPar1LE->setText(QString::number(detector->getYRotationCorrection()     ,'f',4));
    ui->correctionPar2LE->setText(QString::number(detector->getZRotationCorrection()     ,'f',4));
    ui->correctionPar3LE->setText(QString::number(detector->getXPositionCorrection()*10  ,'f',4));
    ui->correctionPar4LE->setText(QString::number(detector->getYPositionCorrection()*10  ,'f',4));
    ui->correctionPar5LE->setText(QString::number(detector->getZPositionCorrection()/1000,'f',4));
    updateTotal();
}

//===========================================================================
bool GeometryParameters::isEnabled(void)
{
    return ui->detectorNameCB->checkState();
}

//===========================================================================
void GeometryParameters::updateTotal()
{
    if(badValue_) return;
    for(int p=0; p<nParameters; p++)
    {
        total_[p]->setText(QString::number(base_[p]->text().toDouble() + correction_[p]->text().toDouble()));
    }
}

//===========================================================================
void GeometryParameters::checkValue(QString text)
{
    badValue_ = true;
    text.toDouble(&badValue_);
    badValue_ = !badValue_;
}

//===========================================================================
double GeometryParameters::getDetectorParTotal(int par)//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
{
    double conversionFactor = 1;
    if(par==3 || par==4) conversionFactor = 0.1;
    else if(par==5) conversionFactor = 1000;
    else if(par>=6 || par<0)
    {
        STDLINE("ERROR:There are only 6 parameters!",ACRed);
        assert(0);
    }
    return total_[par]->text().toDouble()*conversionFactor;
}

//===========================================================================
double GeometryParameters::getDetectorParBase(int par)//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
{
    double conversionFactor = 1;
    if(par==3 || par==4) conversionFactor = 0.1;
    else if(par==5) conversionFactor = 1000;
    else if(par>=6 || par<0)
    {
        STDLINE("ERROR:There are only 6 parameters!",ACRed);
        assert(0);
    }
    return base_[par]->text().toDouble()*conversionFactor;
}

//===========================================================================
double GeometryParameters::getDetectorParCorrection(int par)//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
{
    double conversionFactor = 1;
    if(par==3 || par==4) conversionFactor = 0.1;
    else if(par==5) conversionFactor = 1000;
    else if(par>=6 || par<0)
    {
        STDLINE("ERROR:There are only 6 parameters!",ACRed);
        assert(0);
    }
    return correction_[par]->text().toDouble()*conversionFactor;
}

//===========================================================================
void GeometryParameters::setDetectorParTotal(int par, double value)
{
   total_[par]->setText(QString::number(value));
}

//===========================================================================
void GeometryParameters::setDetectorParBase(int par, double value)
{
    base_[par]->setText(QString::number(value));
}

//===========================================================================
void GeometryParameters::setDetectorParCorrection(int par, double value)
{
    correction_[par]->setText(QString::number(value));
}

//===========================================================================
void GeometryParameters::enable(bool enable)
{
    ui->detectorNameCB->setChecked(enable);
}
