#include <iostream>

#include "builder.h"

#include <QImage>

//=============================================================================================
int builder::execute(const QString & imageFileName)
{
  std::cout << ACGreen << ACBold
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
  for( int i=0; i<10; ++i)
  {
    QImage scaledImage = image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }

  std::cout << ACGreen << ACBold << ACReverse
            << __LINE__
            << "]\t["
            << __PRETTY_FUNCTION__
            << "  --> Done "
            << imageFileName.toStdString()
            << " at "
            << number_
            << ACPlain
            << std::endl ;

  return number_++ ;
}

