wget -T 30 -t 2 "$1" -a /tmp/wgettempfile -O - | tidy -i -asxhtml -latin1 -clean --force-output true > "$2" 2>/tmp/tidyexit
$(dirname $0)/../bin/bt-unicode2ascii "$2" >/dev/null