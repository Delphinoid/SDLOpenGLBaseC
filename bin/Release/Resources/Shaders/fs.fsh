uniform sampler2D MainTex;
uniform sampler2D BumpTex;
uniform sampler2D RampTex;
uniform sampler2D AmbRampTex;
uniform vec3 lightCol;
varying vec3 Normal;
varying vec3 LightDir;
varying vec3 ViewDir;

vec3 linearize(vec3 Input){
    vec3 Output;
    float gamma = 2.2;
    Output.r = pow(Input.r,gamma);
    Output.g = pow(Input.g,gamma);
    Output.b = pow(Input.b,gamma);
    return Output;
}

vec3 srgb(vec3 Input){
    vec3 Output;
    float exp = 1.0/2.2;
    Output.r = pow(Input.r,exp);
    Output.g = pow(Input.g,exp);
    Output.b = pow(Input.b,exp);
    return Output;
}

void main(){
    vec4 col = texture2D(MainTex, gl_TexCoord[0].st);
    col.rgb = linearize(col.rgb);
    
    //normalize
    vec3 viewDir = normalize(ViewDir);
    vec3 lightDir = normalize(LightDir);
    vec3 normal = normalize(texture2D(BumpTex, gl_TexCoord[0].st).xyz * 2.0 - 1.0);
    normal.y = -normal.y; //flip y
    
    //half lambert
    float NdotL = dot(normal,lightDir) * 0.5 + 0.5;
    NdotL = clamp(NdotL,0.01,0.99);
    
    //ramp
    vec3 ramp = texture2D(RampTex, vec2(NdotL)).rgb;
    ramp = linearize(ramp);
    ramp *= 2.0; //adjustment
    ramp *= lightCol;
    
    //fake ambient
    vec3 ambient = texture2D(AmbRampTex, vec2(NdotL)).rgb;
    ambient = linearize(ambient);
    
    //specular
    vec3 halfVector = normalize(lightDir+viewDir);
    float NdotH = max(0.0,dot(normal,halfVector));
    vec3 spec = vec3(1.0,0.9,0.7) * pow(NdotH,50.0);
    spec *= 0.07; //adjustment
    
    //rim
    float NdotE = max(0.0,dot(normal,viewDir));
    vec3 rim = ambient * pow(1.0-NdotE,6.0);
    //rim *= 0.8; //adjustment
    
    col.rgb = col.rgb * (ambient * 0.5 + ramp) + spec + rim;
    col.rgb = srgb(col.rgb);
    
    gl_FragColor = col;
}