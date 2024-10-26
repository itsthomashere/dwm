exec picom -b &
exec pasystray &
exec ibus-daemon -rxRd &
exec nm-applet &
exec nitrogen --restore &

exec ~/suckless/dwm-bar/dwm_bar.sh 2>&1 >/dev/null&

exec dwm
