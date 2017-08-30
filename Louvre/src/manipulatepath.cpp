#include "inc/manipulatepath.h"

//=============================================================================================
QString manipulatePath::getPath(void)
{
    QStringList pathElements = path_.split("/") ;
    QString path = "" ;
    for(int i=0; i<pathElements.size()-1; ++i)
    {
        path += pathElements[i] ;
        path += "/" ;
    }
    return path ;
}
//=============================================================================================
QString manipulatePath::getFileName(void)
{
    QStringList pathElements = path_.split("/") ;
    return pathElements[pathElements.size()-1] ;
}
