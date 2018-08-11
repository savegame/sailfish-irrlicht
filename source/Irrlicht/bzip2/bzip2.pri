DEFINES += BZ_NO_STDIO

HEADERS += \
    $$PWD/bzlib.h \
    $$PWD/bzlib_private.h

SOURCES += \
    $$PWD/blocksort.c \
    $$PWD/bzcompress.c \
#    $$PWD/bzip2.c \
    $$PWD/bzip2recover.c \
    $$PWD/bzlib.c \
    $$PWD/crctable.c \
    $$PWD/decompress.c \
    $$PWD/huffman.c \
#    $$PWD/mk251.c \
    $$PWD/randtable.c \
#    $$PWD/spewG.c \
#    $$PWD/unzcrash.c
