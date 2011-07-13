
TARGET=autoforward.dll
OBJS="autoforward.o version.o"
NAME=autoforward
LIBSYLPH=./lib/libsylph-0-1.a
LIBSYLPHEED=./lib/libsylpheed-plugin-0-1.a
#LIBS=" -lglib-2.0-0  -lintl"
LIBS=" `pkg-config --libs glib-2.0 gobject-2.0 gtk+-2.0`"
INC=" -I. -I../../ -I../../libsylph -I../../src `pkg-config --cflags glib-2.0 cairo gdk-2.0 gtk+-2.0`"
DEF=" -DHAVE_CONFIG_H"

function compile ()
{
    com="gcc -Wall -c $DEF $INC autoforward.c"
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
        DEST="/C/Users/$LOGNAME/AppData/Roaming/Sylpheed/plugins"
        if [ -d "$DEST" ]; then
            com="cp $TARGET $DEST/autoforward.dll"
            echo $com
            eval $com
        else
            DEST="/C/Documents and Settings/$LOGNAME/Application Data/Sylpheed/plugins"
            if [ -d "$DEST" ]; then
                com="cp $TARGET \"$DEST/autoforward.dll\""
                echo $com
                eval $com
            fi
        fi
    fi

}

if [ -z "$1" ]; then
    compile
else
    while [  $# -ne 0 ]; do
        case "$1" in
            -debug|--debug)
                DEF=" -DDEBUG -DHAVE_CONFIG_H"
                shift
                ;;
            pot)
                mkdir -p po
                com="xgettext autoforward.c -k_ -kN_ -o po/autoforward.pot"
                echo $com
                eval $com
                shift
                ;;
            po)
                com="msgmerge po/ja.po po/autoforward.pot -o po/ja.po"
                echo $com
                eval $com
                shift
                ;;
            mo)
                com="msgfmt po/ja.po -o po/autoforward.mo"
                echo $com
                eval $com
                DEST="/C/apps/Sylpheed/lib/locale/ja/LC_MESSAGES"
                if [ -d "$DEST" ]; then
                    com="cp po/$NAME.mo $DEST/autoforward.mo"
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
            -r|release)
                shift
                if [ ! -z "$1" ]; then
                    shift
                    r=$1
                    zip sylpheed-autoforward-$r.zip autoforward.dll
                    zip -r sylpheed-autoforward-$r.zip README.ja.txt
                    zip -r sylpheed-autoforward-$r.zip autoforward.c
                    zip -r sylpheed-autoforward-$r.zip po/autoforward.mo
                    zip -r sylpheed-autoforward-$r.zip *.xpm
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
            *)
                ;;
        esac
    done

fi