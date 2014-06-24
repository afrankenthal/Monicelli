/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "mdisubwindow.h"

#include "MessageTools.h"

//=========================================================================
mdiSubWindow::mdiSubWindow(void)
{
  STDLINE("Building subWindow",ACWhite) ;
}

//=========================================================================
void mdiSubWindow::resizeEvent(QResizeEvent *)
{
  STDLINE("Resizing global subWindow",ACWhite) ;
}
