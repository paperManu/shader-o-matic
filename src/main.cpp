/*
 * Copyright (C) 2013 Emmanuel Durand
 *
 * This file is part of Shader-0-matic.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * blobserver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with blobserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>

#include "shaderomatic.h"

using namespace std;

string gFilename {};
string gObjFilename {};
string gShadername {};
string gResolution {};
int gSwapInterval {1};
bool gWireframe {false};

/*************/
void parseArgs(int argc, char** argv)
{
    for (unsigned int i = 1; i < argc;)
    {
        if ((string(argv[i]) == "--image" || string(argv[i]) == "-i") && i < argc - 1)
        {
            ++i;
            gFilename = string(argv[i]);
        }
        else if ((string(argv[i]) == "--obj" || string(argv[i]) == "-o") && i < argc - 1)
        {
            ++i;
            gObjFilename = string(argv[i]);
        }
        else if ((string(argv[i]) == "--shader" || string(argv[i]) == "-s") && i < argc - 1)
        {
            ++i;
            gShadername = string(argv[i]);
        }
        else if ((string(argv[i]) == "--res" || string(argv[i]) == "-r") && i < argc - 1)
        {
            ++i;
            gResolution = string(argv[i]);
        }
        else if (string(argv[i]) == "--swap" && i < argc - 1)
        {
            ++i;
            gSwapInterval = stoi(string(argv[i]));
        }
        else if (string(argv[i]) == "--wireframe" || string(argv[i]) == "-w")
        {
            gWireframe = true;
        }
        else if (string(argv[i]) == "--help" || string(argv[i]) == "-h")
        {
            cout << "Shader-0-matic, a shader testing tool" << endl;
            cout << endl;
            cout << "Usage:" << endl;
            cout << "-i, --image     \t Specifies the image to use as texture" << endl;
            cout << "-o, --object    \t Specifies the object to load" << endl;
            cout << "-s, --shader    \t Specifies the base name of the shader files (without extension)" << endl;
            cout << "-r, --res       \t Specifies the startup resolution (defaults to 640x480)" << endl;
            cout << "--swap          \t Specifies the frame swap interval" << endl;
            cout << "-w, --wireframe \t Draw objects as wireframe" << endl;
        }
        ++i;
    }
}

/*************/
int main(int argc, char** argv)
{
    shaderomatic app;

    // Parsing the args
    parseArgs(argc, argv);

    if (gFilename != "")
        app.setImageFile(gFilename);
    if (gObjFilename != "")
        app.setObjectFile(gObjFilename);
    if (gShadername != "")
        app.setShaderFile(gShadername);
    if (gResolution != "")
    {
        int w, h;
        w = h = 0;
        sscanf(gResolution.c_str(), "%ix%i", &w, &h);
        app.setResolution(w, h);
    }
    if (gWireframe == true)
    {
        app.setWireframe(true);
    }
    app.setSwapInterval(gSwapInterval);

    app.init();

    return 0;
}

