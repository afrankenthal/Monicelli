#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include <QWidget>
#include <string>
#include <vector>

class Detector;
class QLineEdit;

namespace Ui {
class GeometryParameters;
}

class GeometryParameters : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeometryParameters(QWidget *parent = 0);
    ~GeometryParameters();
    void   showDetectorPars        (Detector* detector);
    double getDetectorParTotal     (int par);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    double getDetectorParBase      (int par);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    double getDetectorParCorrection(int par);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void   setDetectorParTotal     (int par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void   setDetectorParBase      (int par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    void   setDetectorParCorrection(int par, double value);//0=Alpha,1=Beta,2=Gamma,3=X,4=Y,5=Z
    bool   isEnabled               (void);
    void   enable                  (bool enable);

private:
    enum{nParameters=6};
    Ui::GeometryParameters* ui;
    std::vector<QLineEdit*> total_;
    std::vector<QLineEdit*> base_;
    std::vector<QLineEdit*> correction_;
    bool                    badValue_;


private slots:
    void updateTotal(void);
    void checkValue(QString text);
};

#endif // GEOMETRYPARAMETERS_H
