/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx = 2; /* border pixel of windows */
static unsigned int gappx = 8;    /* gaps between windows */
static unsigned int snap = 32;    /* snap pixel */
static const unsigned int systraypinning =
    0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor
          X */
static const unsigned int systrayonleft =
    0; /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 4; /* systray spacing */
static const int systraypinningfailfirst =
    1; /* 1: if pinning fails, display systray on the first monitor, False:
          display systray on the last monitor*/
static const int showsystray = 1; /* 0 means no systray */
static int showbar = 1;           /* 0 means no bar */
static int topbar = 1;            /* 0 means bottom bar */
static char font[] = "Cascadia Code NF:size=14";
static char dmenufont[] = "Cascadia Code NF:size=14";
static const char *fonts[] = {font};
static char normbgcolor[] = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[] = "#bbbbbb";
static char selfgcolor[] = "#eeeeee";
static char selbordercolor[] = "#005577";
static char selbgcolor[] = "#005577";
static char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
static const char *tags[] = {"", "󰒱", "",  "󰝚", "",
                             "", "",  "󰏆", "",  ""};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"Firefox", NULL, NULL, 1 << 8, 0, -1},
    {"Steam", NULL, NULL, 0, 0, -1, 1},
    {"steam_app", NULL, NULL, 0, 0, -1, 1},
};

/* layout(s) */
static float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static int nmaster = 1;     /* number of clients in master area */
static int resizehints = 0; /* 1 means respect size hints in tiled resizals */
static int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static int attachbelow =
    1; /* 1 means attach after the currently active window */

#include "fibonacci.c"
static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[\\]", dwindle}, {"[]=", tile}, /* first entry is default */
    {"><>", NULL}, /* no layout function means floating behavior */
    {"[M]", monocle},  {"[@]", spiral},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
    {MODKEY, KEY, view, {.ui = 1 << TAG}},                                     \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},             \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                      \
        {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
    {                                                                          \
        .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                   \
    }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"rofi", "-show", "drun"};
static const char *druncmd[] = {"rofi", "-show", "run"};
static const char *powermenucmd[] = {
    "rofi",
    "-show",
    "power-menu",
    "-modi",
    "\"power-menu:~/.local/bin/rofi-power-menu "
    "--choices=reboot/shutdown/suspend/logout\"",
    NULL};
static const char *termcmd[] = {"st", "-e", "fish", NULL};
static const char *screenshotcmd[] = {"gnome-screenshot", "-i", NULL};
/* Multimedia keys */
static const char *up_vol[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@",
                               "+5%", NULL};
static const char *down_vol[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@",
                                 "-5%", NULL};
static const char *mute_vol[] = {"pactl", "set-sink-mute", "@DEFAULT_SINK@",
                                 "toggle", NULL};
static const char *next_track[] = {"playerctl", "next", NULL};
static const char *prev_track[] = {"playerctl", "previous", NULL};
static const char *play_pause[] = {"playerctl", "play-pause", NULL};
static const char *bright_up[] = {"brightnessctl", "set", "5%+", NULL};
static const char *bright_down[] = {"brightnessctl", "set", "5%-", NULL};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
    {"dwmfont", STRING, &font},
    {"dmenufont", STRING, &dmenufont},
    {"normbgcolor", STRING, &normbgcolor},
    {"normbordercolor", STRING, &normbordercolor},
    {"normfgcolor", STRING, &normfgcolor},
    {"selbgcolor", STRING, &selbgcolor},
    {"selbordercolor", STRING, &selbordercolor},
    {"selfgcolor", STRING, &selfgcolor},
    {"borderpx", INTEGER, &borderpx},
    {"snap", INTEGER, &snap},
    {"showbar", INTEGER, &showbar},
    {"topbar", INTEGER, &topbar},
    {"nmaster", INTEGER, &nmaster},
    {"resizehints", INTEGER, &resizehints},
    {"mfact", FLOAT, &mfact},
};

#include <X11/XF86keysym.h>
static const Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    {MODKEY | ShiftMask, XK_d, spawn, {.v = druncmd}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_x, spawn, {.v = powermenucmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY, XK_e, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_f, fullscreen, {0}},
    {MODKEY | ShiftMask, XK_q, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_space, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[3]}},
    {MODKEY, XK_r, setlayout, {.v = &layouts[0]}},
    {MODKEY | ShiftMask, XK_r, setlayout, {.v = &layouts[4]}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_h, focusstack, {.i = -1}}, // left
    {MODKEY, XK_l, focusstack, {.i = +1}}, // right
    {MODKEY, XK_j, incnmaster, {.i = +1}}, // down
    {MODKEY, XK_k, incnmaster, {.i = -1}}, // up
    {MODKEY | ShiftMask, XK_h, setmfact, {.f = -0.05}},
    {MODKEY | ShiftMask, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_s, spawn, {.v = screenshotcmd}},
    /* { 0, XF86XK_AudioMute,          spawn, {.v = mute_vol } }, */
    /* { 0, XF86XK_AudioLowerVolume,   spawn, {.v = down_vol } }, */
    /* { 0, XF86XK_AudioRaiseVolume,   spawn, {.v = up_vol } }, */
    {0, XF86XK_AudioPlay, spawn, {.v = play_pause}},
    {0, XF86XK_AudioNext, spawn, {.v = next_track}},
    {0, XF86XK_AudioPrev, spawn, {.v = prev_track}},
    {0, XF86XK_MonBrightnessDown, spawn, {.v = bright_down}},
    {0, XF86XK_MonBrightnessUp, spawn, {.v = bright_up}},
    TAGKEYS(XK_bracketleft, 0) TAGKEYS(XK_bracketright, 1) TAGKEYS(
        XK_braceleft, 2) TAGKEYS(XK_braceright, 3) TAGKEYS(XK_percent, 4)
        TAGKEYS(XK_asciicircum, 5) TAGKEYS(XK_plus, 6) TAGKEYS(XK_asterisk, 7)
            TAGKEYS(XK_parenleft, 8)
                TAGKEYS(XK_parenright, 9){MODKEY | ShiftMask, XK_e, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
