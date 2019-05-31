//
// Created by kinghell on 19-5-29.
//

#pragma once

#include <thread>
#include <string>
#include <iostream>
#include "people/Target.h"

using namespace std;

struct Setting{
    bool keys[1024];
    Target *tars[6];
    Target *tar;
};

class Animation {
public:
    Animation(Setting *setting);

    void start(string name);

private:
    static void running(Setting *setting);

    Setting *setting;
};


