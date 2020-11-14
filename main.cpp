/* 
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"
#include <getopt.h>
#include "h/mem.h"

extern "C" void init_emul();
extern "C" void init_pallete();
extern "C" void open_sna(const char *);
extern "C" void patch_rom(int);

static DrawnWindow *keyPress;
static int mode128k = 0;

const struct option long_options[] = {
    { "128" , no_argument, 0, 0 },
    { 0 , 0, 0, 0 },
};

static void loadromfile(unsigned romidx, const char *name)
{
    QByteArray data;
    const char * p;
    int i;
    QFile file(name);

    if(file.open(QIODevice::ReadOnly)){
        data=file.readAll();
        file.close();
        p=data;
        for (i=0; i < 16384 ; i++)
            emul_writerom_no(romidx, i, *(p++));
    } else {
        printf("Cannot open ROM file %s\n",name);
    }
}

static void setup_window(QApplication &app)
{
    keyPress = new DrawnWindow();
}

static void show_window()
{
    keyPress->show();
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    int option_index;
    int c;

    setup_window(app);
    init_pallete();
    init_emul();

    do {
        c = getopt_long(argc, argv, "",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            mode128k=1;
            break;
        }
    } while (1);

    if (!mode128k) {
        printf("Load 48K ROM\n");
        loadromfile(0, ":/rom/spectrum.rom");
    } else {
        printf("Loading 128K ROMs\n");
        loadromfile(0, ":/rom/128-0.rom");
        loadromfile(1, ":/rom/128-1.rom");
    }

    if (optind < argc) {
        printf("Loading snapshot %s\n", argv[optind]);
        open_sna(argv[optind]);
        optind++;
    }

    show_window();

    return app.exec();
}

