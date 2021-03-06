#version 410 core

in vec4 position_worldSpace;
in vec4 normal_worldSpace;

layout(location=0) out vec4 out0;
layout(location=1) out vec4 out1;
layout(location=2) out vec4 out2;

//take in the world normal and position and save into the color attachments

uniform vec4 materialColor;         // keeps diffuse color and monochromatic spec color

uniform float particleRadius = -1.f;

void main()
{
    if (particleRadius > -0.5f) // calc particle position and normal
    {
        vec4 normal = vec4(0);
        normal.xy = gl_PointCoord * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
        float mag = dot(normal.xy, normal.xy);

        if (mag > 1.0) discard;

        normal.z = sqrt(1.0 - mag);

        out0 = vec4(normal.xyz * particleRadius, 1);
        out1 = normal;
        out2 = vec4(normal.x*.4, .65+normal.y*.3, .77+normal.z*.15,1);
    }
    else // do normal stuff
    {
        out0 = position_worldSpace;
        out1 = normal_worldSpace;
        out2 = vec4(normal_worldSpace.x*.4, .65+normal_worldSpace.y*.3, .77+normal_worldSpace.z*.15,1);
    }


}
