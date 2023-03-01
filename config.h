/* See LICENSE file for copyright and license details. */


#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Cascadia Mono:pixelsize=16" };
static const char dmenufont[]       = "Cascadia Mono:pixelsize=16";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char menubgcolor[]           = "#005577";
static char menufgcolor[]           = "#005577";
static char menuselbordercolor[]    = "#005577";
static char menuselfgcolor[]        = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
/*static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };*/

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance    title           tags mask  isfloating   isterminal  noswallow monitor */
	{ "music_term", NULL,       NULL,           1 << 5,    0,           1,          0,       -1 },
	{ "mpv",        NULL,       NULL,           0,         1,           0,          1,       -1 },
	{ "guake",      NULL,       NULL,           0,         1,           0,          0,       -1 },
	{ "Firefox",    NULL,       NULL,           0,         0,           0,         -1,       -1 },
	{ "st",         NULL,       NULL,           0,         0,           1,          0,       -1 },
	{ NULL,         NULL,       "Event Tester", 0,         0,           0,          1,       -1 }, /* xev */
};

/* layout(s) */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
 	{ "[S]",      spiral },  /* first entry is default */
	{ "[T]",      tile },
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[P]",      monocle },
 	{ "[D]",      dwindle },
};

/* key definitions */
//#define ALTKEY Mod1Mask
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run_history", "-m", dmenumon, "-fn", dmenufont, "-nb", menubgcolor, "-nf", menufgcolor, "-sb", menuselbordercolor, "-sf", menuselfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
// brightness commands
static const char *upbright[] = { "/home/drew/.local/bin/brightness-control", "up", NULL };
static const char *downbright[] = { "/home/drew/.local/bin/brightness-control", "down", NULL };
static const char *upvol[] = { "/usr/bin/amixer", "set", "Master", "5%+", NULL };
static const char *downvol[] = { "/usr/bin/amixer", "set", "Master", "5%-", NULL };
static const char *mutevol[] = { "/usr/bin/amixerl", "set", "Master", "toggle", NULL };
static const char *cmd_emacs[]  = { "emacsclient", "--no-wait", "-create-frame", "--alternate-editor=\"\"", NULL };


/* if those three don't work (with amixer) try these using pactl:
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
*/
/* Didn't work
static const char *altleft[] = { "/home/drew/.local/bin/alt-vimkeys.sh", "m", NULL };
static const char *altdown[] = { "/home/drew/.local/bin/alt-vimkeys.sh", "n", NULL };
static const char *altup[] = { "/home/drew/.local/bin/alt-vimkeys.sh", "e", NULL };
static const char *altright[] = { "/home/drew/.local/bin/alt-vimkeys.sh", "i", NULL };
*/

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
		{ "menubgcolor",        STRING,  &menubgcolor },
		{ "menufgcolor",        STRING,  &menufgcolor },
		{ "menuselbordercolor", STRING,  &menuselbordercolor },
		{ "menuselfgcolor",     STRING,  &menuselfgcolor },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_o,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_z,      spawn,          {.v = termcmd } },
	/*{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },*/
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_s,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_d,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,    			XK_e, 	   spawn,          {.v = cmd_emacs } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    { 0,                            XF86XK_MonBrightnessUp,    spawn,          {.v = upbright  } },
    { 0,                            XF86XK_MonBrightnessDown,  spawn,          {.v = downbright} },
    { 0,                            XF86XK_AudioLowerVolume,   spawn,          {.v = downvol   } },
    { 0,                            XF86XK_AudioMute,          spawn,          {.v = mutevol   } },
    { 0,                            XF86XK_AudioRaiseVolume,   spawn,          {.v = upvol     } },
    /*
    { ALTKEY,                       XK_m,                      spawn,          {.v = altleft  } },
    { ALTKEY,                       XK_n,                      spawn,          {.v = altdown  } },
    { ALTKEY,                       XK_e,                      spawn,          {.v = altup    } },
    { ALTKEY,                       XK_i,                      spawn,          {.v = altright } },
    */
    { ShiftMask,                    XK_Shift_R,                spawn,          SHCMD("setxkbmap -layout clmk-dh") },
    { ShiftMask,                    XK_Shift_L,                spawn,          SHCMD("setxkbmap -layout us") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

