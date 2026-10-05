/*
  RA: the badge in the game list that marks a checked, tracked game.

  After a check the console stores the watch list next to the game as
  `<device>RA/<serial>.wl`. That file's presence is the whole record of
  RA support; there is no separate registry to drift out of sync.

  We look in the same places and in the same order as the loader does
  before launch (`sbLoadWatchList`): the game's own device first, then
  the share. Otherwise the badge would lie, showing what the launch
  cannot find or hiding lists prepared on the PC.

  Recomputed whenever the game list is refreshed, which happens on the
  I/O thread where file operations are safe. It must not be called from
  the menu handler, for the same reason mounting must not.
*/

#include "include/opl.h"
#include "include/util.h"
#include "include/supportbase.h"
#include "include/rabadge.h"
#include "include/rawatch.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define RA_BADGE       "RA "  /* prefix before the name; ASCII only, the stock \
                                 theme font has no other glyphs */
#define PS1_BADGE      "PS1 " /* a POPS image, see sbAppendVcdList */
#define RA_BADGE_SLOTS 4      /* OPL has exactly this many devices: BDM, ETH, HDD, APP */
#define RA_BADGE_TEXT  (ISO_GAME_NAME_MAX + 8)

struct ra_badge_slot
{
    item_list_t *support;
    int count;
    char *text; /* count strings of RA_BADGE_TEXT each, or NULL */
};

static struct ra_badge_slot g_slots[RA_BADGE_SLOTS];

static struct ra_badge_slot *slotFor(item_list_t *support)
{
    int i;

    for (i = 0; i < RA_BADGE_SLOTS; i++)
        if (g_slots[i].support == support)
            return &g_slots[i];

    for (i = 0; i < RA_BADGE_SLOTS; i++) {
        if (g_slots[i].support == NULL) {
            g_slots[i].support = support;
            return &g_slots[i];
        }
    }

    return NULL;
}

static int watchListExists(const char *prefix, const char *serial)
{
    char path[256];
    struct stat st;

    if (serial == NULL || serial[0] == '\0')
        return 0;

    raWatchListPath(path, sizeof(path), prefix, serial);
    if (stat(path, &st) == 0 && st.st_size > 0)
        return 1;

    /* The same fallback the loader uses: lists prepared on the PC live
       on the share. */
    if (strncmp(prefix, "smb0:", 5) != 0) {
        raWatchListPath(path, sizeof(path), "smb0:", serial);
        if (stat(path, &st) == 0 && st.st_size > 0)
            return 1;
    }

    return 0;
}

void raBadgeRefresh(item_list_t *support, int count)
{
    struct ra_badge_slot *slot;
    const char *prefix;
    int i;

    if (support == NULL || support->itemGetName == NULL ||
        support->itemGetStartup == NULL || support->itemGetPrefix == NULL)
        return;

    slot = slotFor(support);
    if (slot == NULL)
        return;

    if (slot->text != NULL) {
        free(slot->text);
        slot->text = NULL;
    }
    slot->count = 0;

    if (count <= 0)
        return;

    prefix = support->itemGetPrefix(support);
    if (prefix == NULL)
        return;

    slot->text = malloc((size_t)count * RA_BADGE_TEXT);
    if (slot->text == NULL)
        return;

    slot->count = count;

    for (i = 0; i < count; i++) {
        char *dst = slot->text + (size_t)i * RA_BADGE_TEXT;
        const char *serial = support->itemGetStartup(support, i);

        const char *ra = watchListExists(prefix, serial) ? RA_BADGE : "";
        const char *ps1 = raItemIsPs1(support, i) ? PS1_BADGE : "";

        if (ra[0] != '\0' || ps1[0] != '\0')
            snprintf(dst, RA_BADGE_TEXT, "%s%s%s", ra, ps1,
                     support->itemGetName(support, i));
        else
            dst[0] = '\0'; /* empty means show the plain name */
    }
}

/* Only BDM pages carry PS1 entries, and only game pages hand out
   base_game_info_t from itemGet; apps do not. */
int raItemIsPs1(item_list_t *support, int idx)
{
    base_game_info_t *g;

    if (support == NULL || support->mode >= ETH_MODE || support->itemGet == NULL)
        return 0;

    g = (base_game_info_t *)support->itemGet(support, idx);
    return g != NULL && g->media == SB_MEDIA_PS1;
}

int raBadgeHas(item_list_t *support, int idx)
{
    return raBadgeText(support, idx) != NULL;
}

const char *raBadgeText(item_list_t *support, int idx)
{
    const struct ra_badge_slot *slot = NULL;
    const char *text;
    int i;

    for (i = 0; i < RA_BADGE_SLOTS; i++)
        if (g_slots[i].support == support)
            slot = &g_slots[i];

    if (slot == NULL || slot->text == NULL || idx < 0 || idx >= slot->count)
        return NULL;

    text = slot->text + (size_t)idx * RA_BADGE_TEXT;

    return text[0] ? text : NULL;
}
