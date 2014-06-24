#include "computation.h"

#include <QImage>

computation::computation(QWidget *)
{
}

int computation::compute(const QString &imageFileName)
{
  const  int imageSize = 100;
  static int counter   = 0 ;
  QImage image(imageFileName);
  QImage scaledImage   = image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  return ++counter ;
}
