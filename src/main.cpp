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

#include <string>

#include "shaderomatic.h"
#include "glib.h"

static gchar* gFilename = NULL;
static gchar* gObjFilename = NULL;
static gchar* gShadername = NULL;
static gchar* gResolution = NULL;
static gint gSwapInterval = 1;
static gboolean gWireframe = FALSE;

static GOptionEntry gEntries[] =
{
    {"image", 'i', 0, G_OPTION_ARG_STRING, &gFilename, "Specifies the image to use as texture.", NULL},
    {"obj", 'o', 0, G_OPTION_ARG_STRING, &gObjFilename, "Specifies the object to load.", NULL},
    {"shader", 's', 0, G_OPTION_ARG_STRING, &gShadername, "Specifies the base name of the shader files (without extension).", NULL},
    {"res", 'r', 0, G_OPTION_ARG_STRING, &gResolution, "Specifies the startup resolution (defaults to 640x480).", NULL},
    {"swap", 0, 0, G_OPTION_ARG_INT, &gSwapInterval, "Specifies the frame swap interval.", NULL},
    {"wireframe", 'w', 0, G_OPTION_ARG_NONE, &gWireframe, "Draw objects as wireframe.", NULL},
    {NULL}
};

using namespace std;

int main(int argc, char** argv)
{
    shaderomatic app;

    // Parsing the args
    GError* error = NULL;
    GOptionContext* context;

    context = g_option_context_new("shader-O-matic, the long waited fragment shader display machine!");
    g_option_context_add_main_entries(context, gEntries, NULL);
    if  (!g_option_context_parse(context, &argc, &argv, &error))
    {
        std::cout << "There were some errors while parsing arguments. How is that possible?" << std::endl;
        return 1;
    }

    if (gFilename != NULL)
        app.setImageFile(string(gFilename));
    if (gObjFilename != NULL)
        app.setObjectFile(string(gObjFilename));
    if (gShadername != NULL)
        app.setShaderFile(gShadername);
    if (gResolution != NULL)
    {
        int w, h;
        w = h = 0;
        sscanf(gResolution, "%ix%i", &w, &h);
        app.setResolution(w, h);
    }
    if (gWireframe == TRUE)
    {
        app.setWireframe(true);
    }
    app.setSwapInterval(gSwapInterval);

    app.init();

    return 0;
}

