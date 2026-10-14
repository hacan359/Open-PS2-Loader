/*
  RetroAchievements support inside the running game. See src/ra.c.
*/
#ifndef RA_H
#define RA_H

/* Takes the watch list, its chains and the snapshot buffer from the
   loader config; the loader placed them in module storage. Call once
   during ee_core init. */
void RA_SetupWatchList(void);

/* Per-frame hook, called from the VBLANK_END interrupt handler. */
void RA_OnVblank(void);

#endif
