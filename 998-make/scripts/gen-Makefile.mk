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

printf "%s_built-lto-y.o: override LD_TXT = \n" "$DIR" >>"$DST".tmp
printf "%s_built-lto-y.o: override LOCAL_LD = \$(LD)\n" "$DIR" >>"$DST".tmp
printf "%s_built-lto-y.o: _empty.lto\n" "$DIR" >>"$DST".tmp
printf "%s_built-obj-y.o: override LD_TXT = \n" "$DIR" >>"$DST".tmp
printf "%s_built-obj-y.o: override LOCAL_LD = \$(LD)\n" "$DIR" >>"$DST".tmp
printf "%s_built-obj-y.o: _empty.nolto\n" "$DIR" >>"$DST".tmp

TGT="lto"
while read TYPE ARG REST; do
    case "$TYPE" in
	("NOLTO")
	    TGT="obj"
	    printf "%s_built-obj-y.o: override LD_TXT = [LTO]\n" "$DIR" >>"$DST".tmp
	    printf "%s_built-obj-y.o: override LOCAL_LD = \$(LD_NO_LTO)\n" "$DIR" >>"$DST".tmp
	    ;;
	(LDSCRIPT)
	    TGT="obj"
	    printf "%s_built-obj-y.o: override LD_TXT = [LTO]\n" "$DIR" >>"$DST".tmp
	    printf "%s_built-obj-y.o: override LOCAL_LD = \$(LD_NO_LTO) -T %sldscript\n" "$DIR" "$DIR" >>"$DST".tmp
	    printf "%s_built-obj-y.o: | %sldscript\n" "$DIR" "$DIR" >>"$DST".tmp
	    [ -d "$DIR" ] || mkdir -p "$DIR"
	    cat > "${DIR}ldscript" <<EOF
SECTIONS
{
    .text$ARG : {
        *(.text)
        *(.text.*)
    }
    .rodata$ARG : {
        *(.rodata)
        *(.rodata.*)
    }
    .data$ARG : {
        *(.data)
        *(.data.*)
    }
    .bss$ARG : {
        *(.bss)
        *(.bss.*)
    }
}
EOF
	    ;;
	("ASFLAGS"|"CFLAGS"|"CXXFLAGS")
	    printf "%s%s: override %s %s\n" "$DIR" "$ARG" "$TYPE" "$REST"
	    printf "%s%s: override %s_TXT = [%s %s]\n" "$DIR" "$ARG" "$TYPE" "$TYPE" "$REST"
	    ;;
	("ASM")
	    REST="$(for OBJ in $REST; do printf " %s%s" "$DIR" "$OBJ"; done)"
	    printf "%s_built-obj-%s.o:%s\n" "$DIR" "$ARG" "$REST"
	    ;;
	("OBJ")
	    REST="$(for OBJ in $REST; do printf " %s%s" "$DIR" "$OBJ"; done)"
	    printf "%s_built-%s-%s.o:%s\n" "$DIR" "$TGT" "$ARG" "$REST"
	    ;;
	("DIR")
	    LTO="$(for OBJ in $REST; do printf " %s%s/_built-lto-y.o" "$DIR" "$OBJ"; done)"
	    printf "%s_built-%s-%s.o:%s\n" "$DIR" "$TGT" "$ARG" "$LTO"
	    OBJ="$(for OBJ in $REST; do printf " %s%s/_built-obj-y.o" "$DIR" "$OBJ"; done)"
	    printf "%s_built-obj-%s.o:%s\n" "$DIR" "$ARG" "$OBJ"
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
