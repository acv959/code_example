#ifndef LOOP_H
#define LOOP_H
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <algorithm>

class Loop{

    static volatile bool quit;
    struct sigaction sa;
    static void got_signal(int);

public:
    Loop();
    int loop();
    void loop(int delay);
};

#endif // LOOP_H
