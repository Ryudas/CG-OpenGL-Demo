#version 420
#extension GL_ARB_explicit_uniform_location : require

layout(std140, binding = 0) uniform CameraUniform {
    mat4 mvp;
    vec3 camera_position;
};

layout(std140, binding = 1) uniform WorldTransformationMatrix {
    mat4 world_transform;
    mat3 normal_transform;
};

layout(std140, binding = 2) uniform LightInformation {
    mat4 light_mvp;
    vec3 light_color;
    vec3 light_position;
    uint casts_shadow;
};

layout(std140, binding = 4) uniform MaterialParameters {
    vec3 ks;
    float shininess;
    vec3 kd;
};
layout(location = 3) uniform sampler2DShadow texShadow;
layout(location = 4) uniform sampler2D tex;
layout(location = 5) uniform sampler2D texToon;

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPosition; // World-space position
in vec3 fragNormal; // World-space normal
in vec2 fragTexCoord; // text coord


void main()
{
    // get normalized light vector
    vec3 lamb_comp = normalize( fragPosition - light_position);

    vec4 fragLightCoord = light_mvp * vec4(fragPosition, 1.0);
    fragLightCoord.xyz /= fragLightCoord.w;
    fragLightCoord.xyz = fragLightCoord.xyz *0.5 + 0.5;
    vec3 shadowMapCoord = fragLightCoord.xyz;
    shadowMapCoord.z -=.001;


    // compute lambertian surface color N.L* C* kd
    lamb_comp = (dot(normalize(fragNormal), lamb_comp)) * light_color * kd;

    // calculate specular component ( reflection vector, incident light vec points to surface)
    vec3 surf_to_camera = normalize(camera_position - fragPosition);
    // calculate halfway vector between viewer and light pos
    vec3 H = normalize( (normalize(light_position - fragPosition)) + (surf_to_camera) );
    // fix for light behind surf
    float dot_p = pow( dot(H, normalize(fragNormal)) , shininess );
    dot_p = max(dot_p, 0);


    vec3 spec_comp = dot_p * ks;


    vec3 final_brightness = abs(spec_comp); //+ abs(lamb_comp);
    float scale_factor = distance(camera_position, light_position );


    float dist_to_frag =  distance(camera_position , fragPosition)  / length(1.25 * camera_position);
    // Output the color from texture
    outColor = vec4(texture( tex , vec2(final_brightness.x, abs(dist_to_frag - 0.2) )).xyz, 1);
}