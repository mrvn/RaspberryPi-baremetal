#!/bin/sh

set -e

SRCDIR="$1"
DST="$2"
SRC="$(printf "%s" "$DST" | sed -e 's,__,/,g' -e 's/.mk$/.obj/' -e 's,^_mk/,,')"

printf "%s: %s\n" "$DST" "$SRC" >"$DST".tmp

DIR="$(dirname "$SRC")/"
if [ "$DIR" = "./" ]; then
    DIR=""
fi

while read TYPE ARG REST; do
    case "$TYPE" in
	("ASFLAGS"|"CFLAGS"|"CXXFLAGS")
	    printf "%s%s: %s %s\n" "$DIR" "$ARG" "$TYPE" "$REST"
	    printf "%s%s: TXT = [%s %s]\n" "$DIR" "$ARG" "$TYPE" "$REST"
	    ;;
	("OBJ")
	    REST="$(for OBJ in $REST; do printf " %s%s" "$DIR" "$OBJ"; done)"
	    printf "%s_built-in-%s.o:%s\n" "$DIR" "$ARG" "$REST"
	    ;;
	("DIR")
	    REST="$(for OBJ in $REST; do printf " %s%s/_built-in-y.o" "$DIR" "$OBJ"; done)"
	    printf "%s_built-in-%s.o:%s\n" "$DIR" "$ARG" "$REST"
	    ;;
	(""|"#"*)
	    : # Empty or comment
	    ;;
	(*)
	    echo >&2 "### unknown"
	    exit 1
	    ;;
    esac
done <"$SRCDIR/$SRC" >>"$DST".tmp
mv "$DST".tmp "$DST"
