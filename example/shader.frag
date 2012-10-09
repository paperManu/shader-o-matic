#version 150 core

uniform sampler2D vTexMap;
uniform sampler2D vHUDMap;
uniform sampler2D vFBOMap;

uniform vec2 vMouse;
uniform float vTimer;
uniform vec2 vResolution;
uniform int vPass;

in vec2 finalTexCoord;

out vec4 fragColor;

vec4 tvScreen(vec4 lIn, sampler2D lTex)
{
    vec4 lOut;

    // add some vertical noise
    int lNoisePosition = int(mod(int(vTimer/3*vResolution.x), int(vResolution.x)));
    int lNoiseValue = 10;
    int lNoiseDrop = 3;

    float lDistance = distance(gl_FragCoord.xy, vec2(lNoisePosition, gl_FragCoord.y));
    vec4 lColor[5];

    if(lDistance < lNoiseDrop)
    {
        lDistance = 1.f - lDistance/lNoiseDrop;
        float lDisplacement = (1.f-abs(lDistance-0.5f)*2.f)*lNoiseValue;
        if(gl_FragCoord.x < lNoisePosition)
        {
            lDisplacement = -lDisplacement;
        }
        //lDisplacement = lDistance*float(lNoiseValue);
        //lOut.r = lDistance;
        lOut = texture(lTex, finalTexCoord.st+vec2(0, lDisplacement/vResolution.y));
    }
    else
    {
        lOut = lIn;
    }

    // turn odd lines to black
    if(mod(floor(gl_FragCoord.y), 2) == 1)
    {
        lOut = vec4(0.f, 0.f, 0.f, 1.f);
    }
    return lOut;
}

vec4 ball()
{
    float lRadius = 20.f;
    vec4 lOut = vec4(0.0, 0.0, 0.0, 0.0);

    float lDistance = distance(gl_FragCoord.xy, vMouse.xy);
    if(lDistance < lRadius)
    {
        lOut = vec4(1.f*(1.f-lDistance/lRadius), 0.f, 0.f, lDistance/lRadius);
    }

    return lOut;
}

void main(void)
{
    if(vPass == 0)
    {
        fragColor = texture(vTexMap, finalTexCoord.st);
        fragColor += ball();
    }
    else if(vPass == 1)
    {
        fragColor = texture(vFBOMap, finalTexCoord.st);
        fragColor = tvScreen(fragColor, vFBOMap);

        // Intégration du HUD
        float lHUDScale = vResolution.y/32.f;
        fragColor += texture(vHUDMap, vec2(finalTexCoord.s, finalTexCoord.t*lHUDScale));
    }
}
