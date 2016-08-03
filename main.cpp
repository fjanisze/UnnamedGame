#include <GL/glut.h>
#include "logger/logger.hpp"
#include "runner/runner.hpp"
#include <iostream>
#include <thread>
#include <cstdio>


int main(int argc,char** argv)
{
    std::ios_base::sync_with_stdio(false);
    SET_LOG_THREAD_NAME("MAIN");

    glutInit(&argc,argv);

    game_runner::runner runner;

    glutMainLoop();
}

