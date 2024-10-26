#include <X11/XF86keysym.h>
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 4;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Cascadia Code NF:size=14" };
static const char dmenufont[]       = "Cascadia Code NF:size=14";
static const char col_gray1[]       = "#1e1e2e";
static const char col_gray2[]       = "#b4befe";
static const char col_gray3[]       = "#cdd6f4";
static const char col_gray4[]       = "#cba6f7";
static const char col_cyan[]        = "#1e1e2e";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_cyan },
	[SchemeSel]  = { col_gray4, col_cyan,  col_gray2 },
};

/* tagging */
static const char *tags[] = { "", "󰒱", "", "󰝚", "", "", "", "󰏆", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
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
static const char *dmenucmd[] = { "rofi", "-show", "drun" };
static const char *powermenucmd[] = {"rofi", "-show", "power-menu", "-modi", "\"power-menu:~/.local/bin/rofi-power-menu --choices=reboot/shutdown/suspend/logout\"", NULL};
static const char *termcmd[]  = { "st", NULL };
/* Multimedia keys */
static const char *up_vol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+1%",   NULL };
static const char *down_vol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-1%",   NULL };
static const char *mute_vol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle", NULL };
static const char *next_track[] = { "playerctl", "next", NULL };
static const char *prev_track[] = { "playerctl", "previous", NULL };
static const char *play_pause[] = { "playerctl", "play-pause", NULL };
static const char *bright_up[] = { "brightnessctl", "set", "5%+", NULL };
static const char *bright_down[] = { "brightnessctl", "set", "5%-", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_x,      spawn,          {.v = powermenucmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_e,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_f,      fullscreen,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
    { MODKEY,                       XK_h,      focusstack,   {.i = -1 } }, // left
    { MODKEY,                       XK_l,      focusstack,   {.i = +1 } }, // right
    { MODKEY,                       XK_j,      incnmaster,   {.i = +1 } }, // down
    { MODKEY,                       XK_k,      incnmaster,   {.i = -1 } }, // up
	{ MODKEY|ShiftMask,             XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_l,      setmfact,       {.f = +0.05} },
    { 0, XF86XK_AudioMute,          spawn, {.v = mute_vol } },
    { 0, XF86XK_AudioLowerVolume,   spawn, {.v = down_vol } },
    { 0, XF86XK_AudioRaiseVolume,   spawn, {.v = up_vol } },
    { 0, XF86XK_AudioPlay,          spawn, {.v = play_pause } },
    { 0, XF86XK_AudioNext,          spawn, {.v = next_track } },
    { 0, XF86XK_AudioPrev,          spawn, {.v = prev_track } },
    { 0, XF86XK_MonBrightnessDown,     spawn, {.v = bright_down } },
    { 0, XF86XK_MonBrightnessUp,     spawn, {.v = bright_up } },
	TAGKEYS(                        XK_exclam,                      0)
	TAGKEYS(                        XK_dollar,                      1)
	TAGKEYS(                        XK_braceleft,                      2)
	TAGKEYS(                        XK_braceright,                      3)
	TAGKEYS(                        XK_percent,                      4)
	TAGKEYS(                        XK_asciicircum,                      5)
	TAGKEYS(                        XK_plus,                      6)
	TAGKEYS(                        XK_asterisk,                      7)
	TAGKEYS(                        XK_parenleft,                      8)
	TAGKEYS(                        XK_parenright,                      9)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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

