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

options=$(getopt -o -hdpm -l debug,po,mo -- "$@")

if [ $? -ne 0 ]; then
    usage
    exit 1
fi
eval set -- "${options}"

TARGET=src/autoforward.dll
OBJS="src/autoforward.o src/version.o"
NAME=autoforward
LIBSYLPH=./lib/libsylph-0-1.a
LIBSYLPHEED=./lib/libsylpheed-plugin-0-1.a
#LIBS=" -lglib-2.0-0  -lintl"
LIBS=" `pkg-config --libs glib-2.0 gobject-2.0 gtk+-2.0`"
INC=" -I. -I../../ -I../../libsylph -I../../src `pkg-config --cflags glib-2.0 cairo gdk-2.0 gtk+-2.0`"
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

    com="gcc -Wall -c -o src/$NAME.o $DEF $INC src/$NAME.c"
    echo $com
    eval $com
    if [ $? != 0 ]; then
        echo "compile error"
        exit
    fi
    com="gcc -shared -o $TARGET $OBJS -L./lib $LIBSYLPH $LIBSYLPHEED $LIBS -lssleay32 -leay32 -lws2_32 -liconv -lonig"
    echo $com
    eval $com
    if [ $? != 0 ]; then
        echo "done"
    else
        if [ -d "$SYLPLUGINDIR" ]; then
            com="cp $TARGET \"$SYLPLUGINDIR/$NAME.dll\""
            echo $com
            eval $com
        else
            :
        fi
    fi

}

if [ -z "$1" ]; then
    compile
else
    while [  $# -ne 0 ]; do
        case "$1" in
            -debug|--debug)
                DEF=" $DEF -DDEBUG"
                shift
                ;;
            pot)
                mkdir -p po
                com="xgettext src/$NAME.c -k_ -kN_ -o po/$NAME.pot"
                echo $com
                eval $com
                shift
                ;;
            po)
                com="msgmerge po/ja.po po/$NAME.pot -o po/ja.po"
                echo $com
                eval $com
                shift
                ;;
            mo)
                com="msgfmt po/ja.po -o po/$NAME.mo"
                echo $com
                eval $com
                if [ -d "$SYLLOCALEDIR" ]; then
                    com="cp po/$NAME.mo \"$SYLLOCALEDIR/$NAME.mo\""
                    echo $com
                    eval $com
                fi
                exit
                ;;
            ui)
                com="gcc -o testui.exe testui.c $INC -L./lib $LIBSYLPH $LIBSYLPHEED $LIBS"
                echo $com
                eval $com
                shift
                ;;
            res)
                com="windres -i version.rc -o version.o"
                echo $com
                eval $com
                shift
                ;;
            -r|release)
                shift
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
            -c|-compile)
                shift
                if [ ! -z "$1" ]; then
                    if [ "$1" = "stable" ]; then
                        DEF="$DEF -DSTABLE_RELEASE";
                        shift
                    fi
                fi
                compile
                ;;
            def)
                shift
                PKG=libsylph-0-1
                com="(cd lib;pexports $PKG.dll > $PKG.dll.def)"
                echo $com
                eval $com
                com="(cd lib;dlltool --dllname $PKG.dll --input-def $PKG.dll.def --output-lib $PKG.a)"
                echo $com
                eval $com
                com="(cd lib;pexports $PKG.dll > $PKG.dll.def)"
                echo $com
                eval $com
                PKG=libsylpheed-plugin-0-1
                com="(cd lib;dlltool --dllname $PKG.dll --input-def $PKG.dll.def --output-lib $PKG.a)"
                echo $com
                eval $com
                exit
                ;;
            clean)
                rm -f *.o *.lo *.la *.bak *~
                shift
                ;;
            cleanall|distclean)
                rm -f *.o *.lo *.la *.bak *.dll *.zip
                shift
                ;;
            *)
                shift
                ;;
        esac
    done

fi
