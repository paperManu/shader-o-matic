#include "shaderomatic.h"
#include "glib.h"

static gchar* gFilename = NULL;
static gchar* gShadername = NULL;
static gchar* gResolution = NULL;
static gint gSwapInterval = 1;
static gboolean gWireframe = FALSE;

static GOptionEntry gEntries[] =
{
    {"image", 'i', 0, G_OPTION_ARG_STRING, &gFilename, "Specifies the image to use as texture (with extension).", NULL},
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
        app.setImageFile(gFilename);
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

