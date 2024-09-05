#pragma once
#include <SDL.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <stack>
#include <vector>
#include "windows.h"
#include <thread>
#include <chrono>

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

const int INSIDE = 0;// 0000
const int LEFT = 1;// 0001
const int RIGHT = 2;// 0010
const int BOTTOM = 4;// 0100
const int TOP = 8;// 1000

static int X_MIN = SCREEN_WIDTH / 2 - 400;
static int Y_MIN = SCREEN_HEIGHT / 2 - 200;
static int X_MAX = SCREEN_WIDTH / 2 + 400;
static int Y_MAX = SCREEN_HEIGHT / 2 + 200;

enum MODE {Inside, Outside, Visible};


#define RGB32(r, g, b) static_cast<uint32_t>((((static_cast<uint32_t>(b) << 8) | g) << 8) | r)



int computeCode(int x, int y);
void line (SDL_Surface *s, int x1, int y1, int x2, int y2, int color);
void put_pixel32(SDL_Surface *s, int x, int y, Uint32 pixel);
Uint32 get_pixel32(SDL_Surface *surface, int x, int y);
void cohenSutherland(float *xstart, float *ystart, float *xend, float *yend, int *flag);


struct Point
{
    int x;
    int y;
};

bool findInStack(std::stack<Point> stack, Point value);

struct Affine_transform
{
    int Tx;
    int Ty;
    float scale;
    float alpha;
};

class Figure
{
    protected:
    int num_vertex;

    std::vector<Point> point;
    Affine_transform koefs;
    glm::mat3 Rotate;
    glm::mat3 Translate;
    glm::mat3 Scale;
    glm::mat3 Transform_matrix;
    public:

    Figure()
    {
        num_vertex=5;
        koefs.Tx=SCREEN_WIDTH/2;
        koefs.Ty=SCREEN_HEIGHT/2;
        koefs.alpha=0;
        koefs.scale=1;

        Rotate = {cos(glm::radians(koefs.alpha)), sin(glm::radians(koefs.alpha)), 0, -sin(glm::radians(koefs.alpha)), cos(glm::radians(koefs.alpha)), 0, 0, 0, 1};
        Translate = {1, 0, koefs.Tx, 0, 1, koefs.Ty, 0, 0, 1};
        Scale ={koefs.scale, 0, 0, 0, koefs.scale, 0, 0, 0, 1};

        Rotate = glm::transpose(Rotate);
        Translate = glm::transpose(Translate);
        Scale = glm::transpose(Scale);

        Transform_matrix=Translate*Rotate*Scale;

    }
    Figure(int x)
    {
        num_vertex=x;
        koefs.Tx=SCREEN_WIDTH/2;
        koefs.Ty=SCREEN_HEIGHT/2;
        koefs.alpha=0;
        koefs.scale=1;

    }

    void set_points()
    {
        Point p;
        p = {0,0};
        point.push_back(p);
        p = {-400, -200};
        point.push_back(p);
        p = {400, -200};
        point.push_back(p);
        p = {400, 200};
        point.push_back(p);
        p = {-400, 200};
        point.push_back(p);
    }




    void draw(SDL_Surface *s)
    {

        for(int i = 1; i < num_vertex; i++)
        {
            if(i == num_vertex-1)
            {
                line(s, point[i].x, point[i].y, point[1].x, point[1].y, RGB32(0, 0, 250));
            }
            else
            {
                line(s, point[i].x, point[i].y, point[i+1].x, point[i+1].y, RGB32(0, 0, 250));
            }
        }
    }

    void get_affine_koef(int flag, float koef)
    {
        switch(flag)
        {
        case 1:
        koefs.Tx -= koef;
        translate();
        break;
        case 2:
        koefs.Ty += koef;
        translate();
        break;
        case 3:
        koefs.alpha += koef;
        rotate();
        break;
        case 4:
        if(koefs.scale+koef > 0.01)
        {koefs.scale += koef; scale(); } //radius+=radius*koef;
        break;
        case 5:
        if(num_vertex+koef>=3)
        {
             for(int j = 0; j < num_vertex; j++)
            {
                point.pop_back();
            }
            num_vertex+=koef;
            for(int i = 0; i < num_vertex; i++)
            {
            Point p;
            p.x = 100*cos(2 * M_PI * i / num_vertex);
            p.y =  100*sin(2 * M_PI * i / num_vertex);
            point.push_back(p);
            }
        }
        break;
        default:
        printf("Wrong affine koef\n");
        break;
        }
    }

    void transform()
    {
        for(int i=0; i<num_vertex; i++)
        {
            glm::vec3 Position = glm::vec3(point[i].x, point[i].y, 1.0f);
            glm::vec3 Transformed = Transform_matrix*Position;
            point[i].x=Transformed.x;
            point[i].y=Transformed.y;
        }
    }

    void translate()
    {
        glm::mat3 Inverse_rotate = glm::inverse(Rotate);
        Translate = glm::inverse(Translate);
        Transform_matrix = Transform_matrix*Inverse_rotate*Translate;
        Translate = {1, 0, koefs.Tx, 0, 1, koefs.Ty, 0, 0, 1};
        Translate = glm::transpose(Translate);
        Transform_matrix = Transform_matrix*Translate*Rotate;
    }

    void rotate()
    {
        Rotate = glm::inverse(Rotate);
        Transform_matrix = Transform_matrix*Rotate;
        Rotate = {cos(glm::radians(koefs.alpha)), sin(glm::radians(koefs.alpha)), 0, -sin(glm::radians(koefs.alpha)), cos(glm::radians(koefs.alpha)), 0, 0, 0, 1};
        Rotate=glm::transpose(Rotate);
        Transform_matrix = Transform_matrix*Rotate;
    }

    void scale()
    {
        Scale = glm::inverse(Scale);
        Transform_matrix = Transform_matrix*Scale;
        Scale = {koefs.scale, 0, 0, 0, koefs.scale, 0, 0, 0, 1};
        Scale = glm::transpose(Scale);
        Transform_matrix = Transform_matrix*Scale;
    }
};

class Circle: public Figure{
    float radius;
    public:

    Circle()
    {
        Point p = {0,0};
        point.push_back(p);
        radius = 30;
        num_vertex = 1;
    }

    void set_points()
    {
        Point p;
        int segments = 360;  // Количество сегментов для приближения окружности
        int centerX = point[0].x;
        int centerY = point[0].y;
        for (int i = 0; i < segments; ++i)
        {
            float angle1 = (i * 2.0f * M_PI) / segments;
            float angle2 = ((i + 1) * 2.0f * M_PI) / segments;

            int x1 = centerX + radius * cos(angle1);
            int y1 = centerY + radius * sin(angle1);
            int x2 = centerX + radius * cos(angle2);
            int y2 = centerY + radius * sin(angle2);
            p = {x1, y1};
            point.push_back(p);
            p = { x2, y2};
            point.push_back(p);
            num_vertex+=2;
        }
    }

    void seedFill(SDL_Surface* s, int startX, int startY, Uint32 newColor, SDL_Texture *gTexture, SDL_Renderer *gRenderer, SDL_Surface *loadedSurface, bool fastFill) {
    // Стек для хранения точек, которые нужно обработать
    std::stack<Point> pixels;
    pixels.push(point[0]);
    int curRadius = radius*koefs.scale;
    while (!pixels.empty()) {
        Point p = pixels.top();
        pixels.pop();

        if(get_pixel32(s, p.x, p.y) == newColor) continue;
        int x = p.x;
        int y = p.y;

        //проверка на расположение относительно окна
        if(x-curRadius+2 >= X_MAX ||
           x+curRadius-2 <= X_MIN ||
           y-curRadius+2 >= Y_MAX ||
           y+curRadius-2 <= Y_MIN){
           break;
        }else{
            if(x >= X_MAX){
                x = (x - curRadius + X_MAX) / 2;
            }
            if(x <= X_MIN){
                x = (x + curRadius + X_MIN) / 2;
            }
            if(y >= Y_MAX){
                y = (y - curRadius + Y_MAX) / 2;
            }
            if(y <= Y_MIN){
                y = (y + curRadius + Y_MIN) / 2;
            }
        }

        Point p2 = {x, y-1};
        if(get_pixel32(s, x, y-1) == get_pixel32(s, x, y))
        {
            pixels.push(p2);
        }
        p2={x+1, y};
        if(get_pixel32(s, x+1, y) == get_pixel32(s, x, y) )
        {
            pixels.push(p2);
        }
        p2 = { x, y + 1};
        if(get_pixel32(s, x, y+1) == get_pixel32(s, x, y) )
        {
            pixels.push(p2);
        }
        p2 = { x-1, y};
        if(get_pixel32(s, x-1, y) == get_pixel32(s, x, y) )
        {
            pixels.push(p2);
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(50)); // задержка 50 мс

        put_pixel32(s, x, y, newColor);

        if(!fastFill){
            SDL_UpdateTexture(gTexture, NULL, loadedSurface->pixels, loadedSurface->pitch);
            SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
            SDL_RenderPresent(gRenderer);
            }
        }
    }


    // void fill(SDL_Surface* s, MODE mode)
    // {
    //     Point point_ztr=point[0];
    //     // int ymin = point[1].y, ymax = point[1].y;
    //     // for (const auto& p : point) {
    //     //     if (p.y < ymin ) ymin = p.y;
    //     //     if (p.y > ymax ) ymax = p.y;
    //     // }
    //     // for (float y = ymin; y <= ymax; ++y) {
    //     //     std::vector<float> intersections;
    //     //     for (int i = 0; i < num_vertex; ++i) {
    //     //         Point p1 = point[i];
    //     //         Point p2 = point[(i + 1) % num_vertex];
    //     //         if (p1.y == p2.y) continue;
    //     //         if (y >= std::min(p1.y, p2.y) && y < std::max(p1.y, p2.y)) {
    //     //             float x = p1.x + (float)(y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
    //     //             intersections.push_back((int)x);
    //     //         }
    //     //     }
    //     //     std::sort(intersections.begin(), intersections.end());
    //     //     float x_start;
    //     //     int flag;
    //     //     float x_end;
    //     //     for (size_t i = 0; i < intersections.size(); i += 2) {
    //     //         x_start = intersections[i];
    //     //         x_end = intersections[i+1];
    //     //         cohenSutherland(&x_start, &y, &x_end, &y, &flag);
    //     //         if(mode==Inside && (flag==1 || flag == 3)) line(s, x_start, y, x_end, y, RGB32(255, 128, 0));
    //     //         else if (mode == Outside && (flag == 0 || flag == 4)) line(s, x_start, y, x_end, y, RGB32(255, 128, 0));
    //     //     }
    //     // }
    // }

    bool isPointInCircle(int x, int y)
    {
        double distance = std::sqrt(std::pow(x - koefs.Tx, 2) + std::pow(y - koefs.Ty, 2));
        return distance <= radius;
    }

    void draw(SDL_Surface * s, MODE mode)
    {
        float x_start;
        float y_start;
        float x_end;
        float y_end;
        float dash_length = 3;
        float gap_length = 1.5;
        int flag;
        for(int i = 1; i < num_vertex; i++)
        {
            if(i == num_vertex-1)
            {
                x_start = point[i].x;
                y_start = point[i].y;
                x_end = point[1].x;
                y_end = point[1].y;
            }
            else
            {
                x_start = point[i].x;
                y_start = point[i].y;
                x_end = point[i+1].x;
                y_end = point[i+1].y;
            }
            cohenSutherland(&x_start, &y_start, &x_end, &y_end, &flag);
            float segment_length = sqrt((x_end - x_start) * (x_end - x_start) + (y_end - y_start) * (y_end - y_start));
            int num_dashes = segment_length / (dash_length + gap_length);
            if(mode==Inside)
            {
                if(flag==1)
                    line(s, x_start, y_start, x_end, y_end, RGB32(0, 255, 0));
                else if(flag==0)
                {

                    for (int j = 0; j < num_dashes; ++j)
                    {
                        float t1 = j * (dash_length + gap_length) / segment_length;
                        float t2 = (j * (dash_length + gap_length) + dash_length) / segment_length;

                        int dash_start_x = x_start + t1 * (x_end - x_start);
                        int dash_start_y = y_start + t1 * (y_end - y_start);
                        int dash_end_x = x_start + t2 * (x_end - x_start);
                        int dash_end_y = y_start + t2 * (y_end - y_start);
                        line(s, x_start, y_start, x_end, y_end, RGB32(148, 24, 248));
                    }
                }
            }
            else
            {
                if(flag==1)
                {
                    for (int j = 0; j < num_dashes; ++j)
                    {
                        float t1 = j * (dash_length + gap_length) / segment_length;
                        float t2 = (j * (dash_length + gap_length) + dash_length) / segment_length;

                        int dash_start_x = x_start + t1 * (x_end - x_start);
                        int dash_start_y = y_start + t1 * (y_end - y_start);
                        int dash_end_x = x_start + t2 * (x_end - x_start);
                        int dash_end_y = y_start + t2 * (y_end - y_start);
                        line(s, x_start, y_start, x_end, y_end, RGB32(148, 24, 248));
                    }
                }
                else if(flag==0)
                    line(s, x_start, y_start, x_end, y_end, RGB32(0, 255, 0));
            }
        }

    }
};
namespace Boo{
class Rectangle: public Figure{
    public:

    void transform()
    {
        Figure::transform();
        X_MIN = point[1].x;
        Y_MIN = point[1].y;
        X_MAX = point[3].x;
        Y_MAX = point[3].y;
    }
};
}

void draw(SDL_Surface *s, Circle, Boo::Rectangle, MODE, bool,  SDL_Texture *gTexture, SDL_Renderer *gRenderer, SDL_Surface *loadedSurface, bool *isFilled);
void circle(SDL_Surface * s, int xc, int yc, int r, int color) ;

