#ifndef FUNDAMENT_COLOR_H
#define FUNDAMENT_COLOR_H

typedef union fn_color fn_color_t;
union fn_color {
    struct {
        float r;
        float g;
        float b;
        float a;
    };

    float rgba[4];
};

// TODO: process rgb.txt to generate this code.
static const fn_color_t fn_alice_blue = {0.94f, 0.97f, 1.f};
static const fn_color_t fn_antique_white = {0.98f, 0.92f, 84.f};
static const fn_color_t fn_cornflower_blue = {0.4f, 0.6f, 0.9f};

#endif //FUNDAMENT_COLOR_H
