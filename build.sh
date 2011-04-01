
TARGET=autoforward.dll
OBJS=autoforward.o
LIBSYLPH=./lib/libsylph-0-1.a
LIBSYLPHEED=./lib/libsylpheed-plugin-0-1.a
#LIBS=" -lglib-2.0-0  -lintl"
LIBS=" `pkg-config --libs glib-2.0 gobject-2.0 gtk+-2.0`"
INC=" -I. -I../../ -I../../libsylph -I../../src `pkg-config --cflags glib-2.0 cairo gdk-2.0 gtk+-2.0`"
DEF=" -DHAVE_CONFIG_H"
if [ -z "$1" ]; then
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
fi

if [ ! -z "$1" ]; then
  case "$1" in
      pot)
          mkdir -p po
          com="xgettext autoforward.c -k_ -kN_ -o po/autoforward.pot"
          ;;
      po)
          com="msgmerge po/ja.po po/autoforward.pot -o po/ja.po"
          ;;
      mo)
          com="msgfmt po/ja.po -o po/autoforward.mo"
          ;;
      ui)
          com="gcc -o testui.exe testui.c $INC -L./lib $LIBSYLPH $LIBSYLPHEED $LIBS"
          ;;
      release)
          zip sylpheed-autoforward-$2.zip autoforward.dll
          zip -r sylpheed-autoforward-$2.zip README.ja.txt
          zip -r sylpheed-autoforward-$2.zip autoforward.c
          zip -r sylpheed-autoforward-$2.zip po/autoforward.mo
          zip -r sylpheed-autoforward-$2.zip *.xpm
          
          ;;
  esac
  echo $com
  eval $com
fi