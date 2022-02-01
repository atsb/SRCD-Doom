//
// CNDOOM demo metadata
//

#include <string.h>
#include "doomtype.h"
#include "d_iwad.h"
#include "i_timer.h"
#include "i_swap.h"
#include "cn_meta.h"
#include "h2def.h"
#include "m_config.h"
#include "m_controls.h"
#include "m_argv.h"
#include "config.h"

boolean     speedparm;
boolean     nmareparm;
boolean     maxparm;
boolean     nm100sparm;
boolean     tysonparm;
boolean     pacifistparm;
boolean     episodeparm;
boolean     movieparm;
boolean     coopparm;
boolean     dmparm;

char        cn_meta_signature[4]    = "CNHN";
char        cn_meta_version[4]      = "0002";

// cndoom, all level times are saved here on map completion for later use,
// also keep track of total time spent on all levels so far.
extern int leveltimes[MAXLEVELTIMES];

static FILE *metafp;

void CN_BindMetaVariables(void)
{
    M_BindStringVariable("cn_meta_id",                &cn_meta_id);
}

static void write_int(FILE *f, int v)
{
	// convert endianness if necessary
	v = LONG(v);
	fwrite(&v,4,1,f);
}

static void write_bytes(FILE *f, const void *buf, int len)
{
	fwrite(buf, 1, len, f);
}

/*
static void CN_AddMetaTag(char *tag, int size, char *data)
{
	int len;

	// Write length of tag name + the name itself.
	len = strlen(tag);
	write_int(metafp, len);
	write_bytes(metafp, tag, len);

	// Write length of data + the data itself.
	//
	// size==0 means it's a string and we can use strlen, otherwise it's
	// binary data and it's the caller's responsibility to set size
	// correctly in advance.

	len = (size) ? size : strlen(data);
	write_int(metafp, len);
	write_bytes(metafp, data, len);
}
*/

void CN_WriteMetaData(char *filename)
{
	int   metapos;	// location of metadata in the lmp file
	int   p;
	char  temp[16];

	//!
	// @category demo
	//
	// Used only when recording speed demo for Competition
	speedparm = M_CheckParm("-speed");

	//!
	// @category demo
	//
	// Used only when recording nightmare demo for Competition
	nmareparm = M_CheckParm("-nmare");

	//!
	// @category demo
	//
	// Used only when recording max demo for Competition
	maxparm = M_CheckParm("-max");

	//!
	// @category demo
	//
	// Used only when recording nightmare 100% secrets demo for Competition
	nm100sparm = M_CheckParm("-nm100s");

	//!
	// @category demo
	//
	// Used only when recording tyson demo for Competition
	tysonparm = M_CheckParm("-tyson");

	//!
	// @category demo
	//
	// Used only when recording pacifist demo for Competition
	pacifistparm = M_CheckParm("-pacifist");

	episodeparm = M_CheckParm("-episode");

	//!
	// @category demo
	//
	// Used only when recording movie demo for Competition
	movieparm = M_CheckParm("-movie");

    dmparm = M_CheckParm("-deathmatch");

    // check if game is DM or not
    if (!dmparm)
        coopparm = M_CheckParm ("-connect");

    metafp = fopen(filename, "rb+");
    if (!metafp)
	return;

    // seek to end of demo first and save the location so it can be written later
    fseek (metafp, 0, SEEK_END);
    metapos = ftell(metafp);

    // fputc (012, metafp);
    // META HEADER
    write_bytes(metafp, "*", 1);

    // write signature "CNDM"
    write_bytes(metafp, cn_meta_signature, 4);
    write_bytes(metafp, "#", 1);

    // write meta version
    write_bytes(metafp, cn_meta_version, 4);
    write_bytes(metafp, "#", 1);

    // write CNDOOM version
    write_bytes(metafp, PACKAGE_VERSION, 7);
    write_bytes(metafp, "#", 1);

    // write Competition ID
    write_bytes(metafp, cn_meta_id, 4);
    write_bytes(metafp, "#", 1);

    // player2 ID
    write_bytes(metafp, "0000", 4);
    write_bytes(metafp, "#", 1);

    // player3 ID
    write_bytes(metafp, "0000", 4);
    write_bytes(metafp, "#", 1);

    // player4 ID
    write_bytes(metafp, "0000", 4);
    write_bytes(metafp, "#", 1);

    write_bytes(metafp, "#", 1);

    // write episode
	sprintf(temp, "%d", gameepisode);
	write_bytes(metafp, temp, 1);
    write_bytes(metafp, "#", 1);

    write_bytes(metafp, temp, 3);
    write_bytes(metafp, "#", 1);

    // write category
    write_bytes(metafp,
	(nomonsters)   ? "31" :
	(speedparm)    ? "01" :
	(nmareparm)    ? "02" :
	(maxparm)      ? "03" :
	(nm100sparm)   ? "04" :
	(tysonparm)    ? "05" :
	(pacifistparm) ? "06" :
	(respawnparm)  ? "08" :
	                 "00", 2);
    write_bytes(metafp, "#", 1);

    // check episode or movie
    write_bytes(metafp,
	(episodeparm) ? "E" :
	(movieparm)   ? "M" :
			                "D", 1);
    write_bytes(metafp, "#", 1);

    // check sp, coop, dm
    write_bytes(metafp,
	(dmparm)    ? "D" :
	(coopparm)  ? "C" :
			              "S", 1);
    write_bytes(metafp, "#", 1);

    // write skill
    write_bytes(metafp,
	(gameskill == sk_baby)      ? "1" :
	(gameskill == sk_easy)      ? "2" :
	(gameskill == sk_medium)    ? "3" :
	(gameskill == sk_hard)      ? "4" :
	(gameskill == sk_nightmare) ? "5" :
			                              "0", 1);
    write_bytes(metafp, "#", 1);

    // write game stats (valid only for one map)
    write_bytes(metafp, "#", 1);
	write_bytes(metafp, "XX:XX:XX.XX", 11);
	write_bytes(metafp, "#", 1);

    // META FOOTER
    write_bytes(metafp, "*", 1);

    // finally write metadata location and # of tags
    write_int(metafp, metapos);

    fclose(metafp);
}
