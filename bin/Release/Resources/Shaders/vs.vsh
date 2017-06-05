attribute vec4 Tangent;
uniform vec3 worldLightDir;
uniform mat4 ViewMatrix;
varying vec3 Normal;
varying vec3 LightDir;
varying vec3 ViewDir;

void main(){
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    //view space first
    vec3 viewDir = -normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 lightDir = normalize(ViewMatrix * vec4(worldLightDir,0.0)).xyz;
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 tangent = normalize(gl_NormalMatrix * Tangent.xyz);
    vec3 binormal = cross(normal,tangent);
    
    //then tangent space
    ViewDir.x = dot(tangent,viewDir);
    ViewDir.y = dot(binormal,viewDir);
    ViewDir.z = dot(normal,viewDir);
    LightDir.x = dot(tangent,lightDir);
    LightDir.y = dot(binormal,lightDir);
    LightDir.z = dot(normal,lightDir);
    
}