/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

/*! \file msg.h
 *  \brief (Documentation in progress)
 *
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
 
#ifndef __MSG
#define __MSG

#define MAX_CMD_LENGHT   256
#define MAX_NAME_LENGHT   80
#define LOWEST_PRIORITY    0
#define LOW_PRIORITY       1
#define MEDIUM_PRIORITY    2
#define HIGH_PRIORITY      3
#define HIGHEST_PRIORITY   4

//====================================================================================
/*! \struct msg 
 *  \brief (Documentation in progress)
 *  
 *  \image html  separator.gif
 *  \image latex separator.pdf
 */
namespace Monicelli
{
struct msg 
{
  long         to                          ;
  long         from                        ;
  char         classname  [MAX_NAME_LENGHT];
  char         methodname [MAX_NAME_LENGHT];
  char         command    [MAX_CMD_LENGHT] ;
  char         args       [MAX_CMD_LENGHT] ;
  unsigned int priority                    ;
};
}
#endif
