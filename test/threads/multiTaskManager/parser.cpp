#include <iostream>

#include "parser.h"

#include <QImage>

//=============================================================================================
int parser::execute(const QString & imageFileName)
{
  std::cout << ACYellow << ACBold
            << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__ << std::endl ;

  std::cout << ACYellow << ACBold
            << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "]\timage: "
            << imageFileName.toStdString()
            << " at "
            << number_
            << "..."
            << ACPlain
            << std::endl ;

  const  int imageSize = 100;
  QImage image(imageFileName);
  for( int i=0; i<5; ++i)
  {
    QImage scaledImage = image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }

  std::cout << ACYellow << ACBold << ACReverse
            << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "  --> Done "
            << imageFileName.toStdString()
            << " at "
            << number_
            << ACPlain
            << std::endl ;

  return number_ ;
}

