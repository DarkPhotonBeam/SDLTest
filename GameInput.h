//
// Created by dan on 11/12/24.
//

#ifndef GAMEINPUT_H
#define GAMEINPUT_H

struct Game_Input {
    double xAxis = 0.0;
    double yAxis = 0.0;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool quit = false;
    bool space = false;
    bool r = false;
};

#endif //GAMEINPUT_H
