#include "shaderomatic.h"
#include "glib.h"

static gboolean gVersion = FALSE;
static gchar* gFilename = NULL;
static gchar* gShadername = NULL;
static gint gSwapInterval = 1;

static GOptionEntry gEntries[] =
{
    {"version", 'v', 0, G_OPTION_ARG_NONE, &gVersion, "Shows version of shader-o-matic.", NULL},
    {"image", 'i', 0, G_OPTION_ARG_STRING, &gFilename, "Specifies the image to use as texture (with extension).", NULL},
    {"shader", 's', 0, G_OPTION_ARG_STRING, &gShadername, "Specifies the base name of the shader files (without extension).", NULL},
    {"swap", 0, 0, G_OPTION_ARG_INT, &gSwapInterval, "Specifies the frame swap interval.", NULL},
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
    if(!g_option_context_parse(context, &argc, &argv, &error))
    {
        std::cout << "There were some errors while parsing arguments. How is that possible?" << std::endl;
        return 1;
    }

    if(gFilename != NULL)
        app.setImageFile(gFilename);
    if(gShadername != NULL)
        app.setShaderFile(gShadername);
    app.setSwapInterval(gSwapInterval);

    app.init();

    return 0;
}

