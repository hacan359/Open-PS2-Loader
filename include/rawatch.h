/* RA: the watch list for the game being launched. See src/rawatch.c */
#ifndef __RAWATCH_H__
#define __RAWATCH_H__

#include "modules/network/common/ra_watch.h"

/* "<prefix>RA/<serial>.wl": where a game's list lives on a device. The
   loader, the badge and the network save all use this one. */
void raWatchListPath(char *out, int sz, const char *prefix, const char *serial);
int LoadWatchList(const char *path, const char *startup);
/* One line in the debug launch log: what happened plus two numbers.
   Independent of the hash log, which is already closed by launch time. */
void raLaunchNote(const char *what, int a, int b);
/* Takes the list from memory: the network brought it and the file may
   not have reached the medium yet. Records which game it belongs to. */
int SetWatchList(const void *data, int len, const char *startup);
unsigned int *GetWatchList(void);
int GetWatchCount(void);
int GetWatchBytes(void);
/* The pointer chains that came with the list, already checked: every
   parent points backwards, so ee_core can resolve them in one pass. */
struct ra_node *GetNodeList(void);
int GetNodeCount(void);
void ClearWatchList(void);

/* Copies the list, its chains and room for the snapshot into a block
   at `at` (rounded up to 64 bytes) and returns the first byte after
   it, or `at` unchanged when there is no list. Called with the end of
   module storage, so the block extends it: the kernel wipes memory
   from that end on, and the game never sees the block as free. */
void *PlaceWatchBlock(void *at);
/* Where the block put things, NULL without a list. */
u32 *GetWatchBlockList(void);
struct ra_node *GetWatchBlockNodes(void);
void *GetWatchBlockSnap(void);

#endif
