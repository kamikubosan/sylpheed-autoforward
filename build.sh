#!/bin/bash

run() {
    "$@"
    if test $? -ne 0; then
        echo "Failed $@"
        exit 1
    fi
}

usage() {
    cat <<EOF 1>&2
Usage:
     $0 [-d|--debug]
        [-p|--po]
        [-m|--mo]
Mandatory args:
  -d,--debug enable debug build
  -p,--po    update po files
  -m,--mo    update mo files
Optional args:
  -h,--help  print this help
EOF
}

make_def() {
    for pkg in libsylph-0-1 libsylpheed-plugin-0-1; do
	(cd lib;pexports $pkg.dll > $pkg.dll.def)
	(cd lib;dlltool --dllname $pkg.dll --input-def $pkg.dll.def --output-lib $pkg.a)
    done
}

make_clean() {
    rm -f *.o *.lo *.la *.bak *~
}

make_distclean() {
    rm -f *.o *.lo *.la *.bak *.dll *.zip
}

make_mo() {
    run msgfmt po/ja.po -o po/$NAME.mo
    if [ -d "$SYLLOCALEDIR" ]; then
        run cp po/$NAME.mo $SYLLOCALEDIR/$NAME.mo
    fi
}

mode=""
options=$(getopt -o -hdpm -l debug,po,mo,def -- "$@")

if [ $? -ne 0 ]; then
    usage
    exit 1
fi
eval set -- "${options}"

while true
do
    case "$1" in
        -h|--help)   usage && exit 0;;
        -d|--debug) mode=debug; shift;;
        -p|--po)    mode=po; shift;;
        -m|--mo) make_mo; shift;;
        --def)
            make_def; shift;;
        --dclean)
	    make_distclean; shift;;
	--clean)
	    make_clean; shift;;
	*)
            break
	    ;;
    esac
done

TARGET=src/autoforward.dll
OBJS="src/autoforward.o src/version.o"
NAME=autoforward
LIBSYLPH=./lib/libsylph-0-1.a
LIBSYLPHEED=./lib/libsylpheed-plugin-0-1.a
#LIBS=" -lglib-2.0-0  -lintl"
LIBS=" `pkg-config --libs glib-2.0 gobject-2.0 gtk+-2.0`"
INC=" -I. -I./src -I./lib/sylplugin_factory/src -I../../ -I../../libsylph -I../../src  `pkg-config --cflags glib-2.0 cairo gdk-2.0 gtk+-2.0`"
DEF=" -DHAVE_CONFIG_H"

PBUILDH="src/private_build.h" 
DCOMPILE="src/.compile"


MAJOR=0
MINOR=7
SUBMINOR=0


function compile ()
{
    if [ ! -f "$PBUILDH" ]; then
        echo "1" > $DCOMPILE
        echo "#define PRIVATE_BUILD 1" > $PBUILDH
    else
        ret=`cat $DCOMPILE | gawk '{print $i+1}'`
        echo $ret | tee $DCOMPILE
        echo "#define PRIVATE_BUILD \"build $ret\\0\"" > $PBUILDH
        echo "#define NAME \"Autoforward\\0\"" >> $PBUILDH
        echo "#define VERSION \"$MAJOR, $MINOR, $SUBMINOR, 0\\0\"" >> $PBUILDH
        echo "#define NAMEVERSION \"Autoforward $MAJOR.$MINOR.$SUBMINOR\\0\"" >> $PBUILDH
        echo "#define QVERSION \"$MAJOR,$MINOR,$SUBMINOR,0\"" >> $PBUILDH
        echo "#define RVERSION $MAJOR,$MINOR,$SUBMINOR,0" >> $PBUILDH
    fi
    com="windres -i res/version.rc -o src/version.o"
    echo $com
    eval $com

    for src in `find . -name '*.c'`; do
	src_base=${src%%.c}
	run gcc -Wall -c -o ${src_base}.o $DEF $INC ${src}
    done

    OBJS=`find . -name '*.o'`
    run gcc -shared -o $TARGET $OBJS -L./lib $LIBSYLPH $LIBSYLPHEED $LIBS -lssleay32 -leay32 -lws2_32 -liconv -lonig
    if [ -d "$SYLPLUGINDIR" ]; then
        com="cp $TARGET \"$SYLPLUGINDIR/$NAME.dll\""
        echo $com
        eval $com
    else
        :
    fi
}

case $mode in

    debug)
        DEF=" $DEF -DDEBUG"
	compile
	;;
    pot)
        mkdir -p po
	run xgettext src/$NAME.c -k_ -kN_ -o po/$NAME.pot
	;;
    po)
        run msgmerge po/ja.po po/$NAME.pot -o po/ja.po
	;;
    ui)
        run gcc -o testui.exe testui.c $INC -L./lib $LIBSYLPH $LIBSYLPHEED $LIBS
        ;;
    res)
        run windres -i version.rc -o version.o
	;;
    release)
	if [ ! -z "$1" ]; then
	    r=$1
	    shift
            if [ -f src/$NAME.dll ]; then
		mv src/$NAME.dll .
	    fi
	    zip sylpheed-$NAME-${r}.zip $NAME.dll
            zip -r sylpheed-$NAME-$r.zip doc/README.ja.txt
            zip -r sylpheed-$NAME-$r.zip src/*.h
            zip -r sylpheed-$NAME-$r.zip src/auto*.c
            zip -r sylpheed-$NAME-$r.zip res/*.rc
            zip -r sylpheed-$NAME-$r.zip po/$NAME.mo
            zip -r sylpheed-$NAME-$r.zip res/*.xpm
            zip -r sylpheed-$NAME-$r.zip COPYING
            zip -r sylpheed-$NAME-$r.zip LICENSE
            zip -r sylpheed-$NAME-$r.zip README.md
            zip -r sylpheed-$NAME-$r.zip NEWS
            sha1sum sylpheed-$NAME-$r.zip > sylpheed-$NAME-$r.zip.sha1sum
        fi
	;;
    compile)
        if [ ! -z "$1" ]; then
            if [ "$1" = "stable" ]; then
                DEF="$DEF -DSTABLE_RELEASE";
                shift
            fi
        fi
        compile
        ;;
    *)
	compile
	;;
esac
