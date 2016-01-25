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

