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

    typedef  std::vector<std::string> vStringDef ;

    void       clearTable  (void                     );
    void       initialize  (void                     );
    void       addDetector (std::string   detector,
                            double        zPosition  );
    void       post        (void                     );
    bool       isFixed     (std::string   detector,
                            std::string   parameter  );
    vStringDef getHeaders  (void                     ){return columnHeaderText_;}

  public slots:
    void       fixAll      (int           state      );
    void       fixExtremes (int           state      );
    void       fixAllZ     (int           state      );
    void       fixAllAngles(int           state      );

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
