/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QTableView>
#include <QDomNodeList>

class QRect;
class QTableWidgetItem;

namespace Ui {
class customTableWidget;
}

class customTableWidget : public QTableView
{
    Q_OBJECT

public:
    explicit customTableWidget(QWidget* parent = 0 );
    ~customTableWidget        (void                );

    void              init                    (void                            );
    void              assignXmlElement        (QDomNodeList&  element          );
    int 	          columnCount             (void                            ) const;
    void              setColumnCount          (int col                         );
    void              setHorizontalHeaderItem (int col, QTableWidgetItem * item);
    QTableWidgetItem* horizontalHeaderItem    (int col                         ) const;

private:
    Ui::customTableWidget* ui          ;
    QWidget*               theParent_  ;
    QDomNodeList           elementNode_;
public slots:
    void setEnabled   (int  enable      );
    void setEnabled   (bool enable      );
private slots:
    void setNodeValue (int  row, int col);

signals:
    void cellChanged  (int  row, int col);
};

#endif //CUSTOMTABLEWIDGET_H
