#ifndef customTableView_H
#define customTableView_H

#include <sstream>
#include <string>
#include <QTableView>
#include <QWidget>
#include <QItemDelegate>
#include <QStandardItemModel>

namespace Ui {
    class customTableView;
}

class customTableView : public QTableView
{
    Q_OBJECT

  public:
    explicit customTableView(QWidget * parent = 0 );
            ~customTableView(void                 );
    void      getIsStripCustomTableView(bool isStrip) {isStripCustomTableView_ = isStrip;}
    bool      getIsStripCustomTableView(void) {return isStripCustomTableView_;}


    typedef  std::vector<std::string> vStringDef ;

    void       clearTable  (void                     );
    void       initialize  (void                     );
    void       addDetector (std::string   detector,
                            double        zPosition  );
    void       post        (void                     );
    bool       isFixed     (std::string   detector,
                            std::string   parameter  );
    vStringDef getHeaders  (void                     ){return columnHeaderText_;}
    int        rowCount    (void                     ){return model_->rowCount();}

  public slots:
    void       fixAll           (int           state          );
    void       fixExtremes      (int           state          );
    void       fixAllX          (int           state          );
    void       fixAllY          (int           state          );
    void       fixAllZ          (int           state          );
    void       fixAllTrans      (int           state          );
    void       fixAllAngles     (int           state          );
    void       fixAllAlpha      (int           state          );
    void       fixAllBeta       (int           state          );
    void       fixAllGamma      (int           state          );
    void       enableAll        (int           state          );

    void       fixXStrip   (int state, int           row        );
    void       fixYStrip   (int state, int           row        );

  private slots:
    void       clicked     (QModelIndex   mIndex     );

  private:
    Ui::customTableView * ui  ;

    void       dump        (void                     );
    \
    QWidget             * parent_ ;
    QStandardItemModel  * model_  ;

    int maxCols_ ;
    int maxRows_ ;
    bool isStripCustomTableView_;


    std::stringstream     ss_ ;

    vStringDef                    columnHeaderText_ ;
    vStringDef                    orderedDetectors_ ;
    std::map<double, std::string> detectors_ ;

};

//================================================================================================
class Delegate : public QItemDelegate
{
  public:
      QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index )  const;

  private:

};

#endif // customTableView_H
