/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx        = 3;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar                  = 1;        /* 0 means no bar */
static int topbar                   = 1;        /* 0 means bottom bar */
static char font[]                  = "monospace:pixelsize=13:antialias=true:autohint=true";
static char dmenufont[]             = "monospace:pixelsize=13:antialias=true:autohint=true";
static const char *fonts[]          = {
  font,
  "Symbols Nerd Font:pixelsize=14:antialias=true:autohint=true",
  "Noto Color Emoji:pixelsize=12:antialias=true:autohint=true",
  "Noto Sans CJK JP:pixelsize=13:antialias=true:autohint=true",
};
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "", "󰈹", "󰇰", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                instance          title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",               NULL,             NULL,           0,         1,          0,           0,        -1 },
	{ "firefox",            NULL,             NULL,           1 << 1,    0,          0,           0,        -1 },
	{ "St",                 NULL,             NULL,           0,         0,          1,           0,        -1 },
	{ "St",                 "float-term",     NULL,           0,         1,          1,           0,        -1 },
	{ NULL,                 NULL,             "Event Tester", 0,         1,          0,           1,        -1 }, /* xev */
	{ "mullvadbrowser",     NULL,             NULL,           1 << 1,    0,          0,           0,        -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ Mod1Mask,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ Mod1Mask|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ Mod1Mask|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ Mod1Mask|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
/* r-n-v in dvorak is top-middle-bottom*/
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
  { MOD, XK_r,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_n,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_v,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "slstatus"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-i", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
  { "font",               STRING,  &font },
  { "dmenufont",          STRING,  &dmenufont },
  { "normbgcolor",        STRING,  &normbgcolor },
  { "normbordercolor",    STRING,  &normbordercolor },
  { "normfgcolor",        STRING,  &normfgcolor },
  { "selbgcolor",         STRING,  &selbgcolor },
  { "selbordercolor",     STRING,  &selbordercolor },
  { "selfgcolor",         STRING,  &selfgcolor },
  { "borderpx",          	INTEGER, &borderpx },
  { "snap",          		  INTEGER, &snap },
  { "showbar",          	INTEGER, &showbar },
  { "topbar",          	  INTEGER, &topbar },
  { "nmaster",          	INTEGER, &nmaster },
  { "resizehints",       	INTEGER, &resizehints },
  { "mfact",      	 	    FLOAT,   &mfact },
};

#include "shiftview.c"
static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ Mod1Mask|ShiftMask,           XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
  { MODKEY|ControlMask,           XK_b,      toggleborder,   {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ControlMask,              push)
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	// { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	// { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	// { MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.05} },
	// { MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.05} },
	// { MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	// { MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	// { MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },
	// { MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_g,  togglegaps,     {0} },
	// { MODKEY|ShiftMask,             XK_0,      defaultgaps,    {0} },
	{ Mod1Mask,                     XK_Tab,    view,           {0} },
	{ Mod1Mask|ShiftMask,           XK_c,      killclient,     {0} },

	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]}  },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[1]}  },
	{ MODKEY|ShiftMask,             XK_s,      setlayout,      {.v = &layouts[2]}  },
	{ MODKEY|ShiftMask,             XK_d,      setlayout,      {.v = &layouts[3]}  },
	{ MODKEY|ShiftMask,             XK_h,      setlayout,      {.v = &layouts[4]}  },
  { MODKEY|ShiftMask,             XK_b,      setlayout,      {.v = &layouts[5]}  },
  { MODKEY|ShiftMask,             XK_g,      setlayout,      {.v = &layouts[7]}  },
	{ MODKEY|ShiftMask,             XK_c,      setlayout,      {.v = &layouts[11]} },
	{ Mod1Mask,                     XK_space,  setlayout,      {0} },
	{ Mod1Mask|ShiftMask,           XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_s,      togglesticky,   {0} },

	{ MODKEY,                       XK_Down,   moveresize,     {.v = "0x 10y 0w 0h" } },
	{ MODKEY,                       XK_Up,     moveresize,     {.v = "0x -10y 0w 0h" } },
	{ MODKEY,                       XK_Right,  moveresize,     {.v = "10x 0y 0w 0h" } },
	{ MODKEY,                       XK_Left,   moveresize,     {.v = "-10x 0y 0w 0h" } },
	// { MODKEY|ShiftMask,             XK_Down,   moveresize,     {.v = "0x 0y 0w 10h" } },
	// { MODKEY|ShiftMask,             XK_Up,     moveresize,     {.v = "0x 0y 0w -10h" } },
	// { MODKEY|ShiftMask,             XK_Right,  moveresize,     {.v = "0x 0y 10w 0h" } },
	// { MODKEY|ShiftMask,             XK_Left,   moveresize,     {.v = "0x 0y -10w 0h" } },
	{ MODKEY|ControlMask,           XK_Up,     moveresizeedge, {.v = "t"} },
	{ MODKEY|ControlMask,           XK_Down,   moveresizeedge, {.v = "b"} },
	{ MODKEY|ControlMask,           XK_Left,   moveresizeedge, {.v = "l"} },
	{ MODKEY|ControlMask,           XK_Right,  moveresizeedge, {.v = "r"} },
	// { MODKEY|ControlMask|ShiftMask, XK_Up,     moveresizeedge, {.v = "T"} },
	// { MODKEY|ControlMask|ShiftMask, XK_Down,   moveresizeedge, {.v = "B"} },
	// { MODKEY|ControlMask|ShiftMask, XK_Left,   moveresizeedge, {.v = "L"} },
	// { MODKEY|ControlMask|ShiftMask, XK_Right,  moveresizeedge, {.v = "R"} },
	{ MODKEY,                       XK_c,      movecenter,     {0} },

	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	// { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	// { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	// { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	// { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ Mod1Mask|ControlMask,           XK_comma,  shiftview,    {.i = -1 } },
	{ Mod1Mask|ControlMask,           XK_period, shiftview,    {.i = +1 } },

/* ampersand(1) - bracketleft(2) - braceleft(3) - braceright(4) - parenleft(5) - equal(6) - asterisk(7) - parenright(8) - plus(9) - bracketright(0) */
	TAGKEYS(                        XK_ampersand,          0)
	TAGKEYS(                        XK_bracketleft,        1)
	TAGKEYS(                        XK_braceleft,          2)
	TAGKEYS(                        XK_braceright,         3)
	TAGKEYS(                        XK_parenleft,          4)
	TAGKEYS(                        XK_equal,              5)
	TAGKEYS(                        XK_asterisk,           6)
	TAGKEYS(                        XK_parenright,         7)
	TAGKEYS(                        XK_plus,               8)
	{ MODKEY|ShiftMask,             XK_BackSpace, quit,   {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

