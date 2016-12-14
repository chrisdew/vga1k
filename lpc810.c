/*
 * This is the code which is only compiled for the LPC810.
 */

#include "lpc810.h"

/* hold this idea in reserve
union cpoles_or_panels cpoles_or_panels;
union spoles_or_rles spoles_or_rles;
*/
static struct cpole cpoles[MAX_WALLS_PLUS_ONE];
static struct spole spoles[MAX_WALLS_PLUS_ONE];
static struct panel panels[MAX_WALLS_PLUS_ONE];
static struct rle rles[MAX_COLOUR_CHANGES];


