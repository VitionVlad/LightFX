#version 400 core

in vec2 uv;

out vec4 FragColor;

uniform vec2 resolution;

uniform vec2 rotation;

uniform vec3 position;

uniform vec4 random;

uniform vec3 Mesh[1000];

uniform ivec3 MeshIndices[1000];

uniform int totalv;

uniform ivec2 Settings;

vec2 hash22(vec2 p)
{
	p += random.x;
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 randomOnSphere() {
    vec2 locuv = gl_FragCoord.xy / resolution;
	vec2 uvRes = hash22(locuv + 1.0) * resolution + resolution;
	vec3 rand = vec3(rand(uvRes), rand(-uvRes), rand(vec2(1.0)-uvRes));
	float theta = rand.x * 2.0 * 3.14159265;
	float v = rand.y;
	float phi = acos(2.0 * v - 1.0);
	float r = pow(rand.z, 1.0 / 3.0);
	float x = r * sin(phi) * cos(theta);
	float y = r * sin(phi) * sin(theta);
	float z = r * cos(phi);
	return vec3(x, y, z);
}

mat2 rot(float a) {
	float s = sin(a);
	float c = cos(a);
	return mat2(c, -s, s, c);
}

vec2 boxIntersection(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN)  {
	vec3 m = 1.0 / rd;
	vec3 n = m * ro;
	vec3 k = abs(m) * rad;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2(-1.0);
	oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	return vec2(tN, tF);
}

vec3 triIntersect( in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2)
{
    vec3 v1v0 = v1 - v0;
    vec3 v2v0 = v2 - v0;
    vec3 rov0 = ro - v0;
    vec3  n = cross( v1v0, v2v0 );
    vec3  q = cross( rov0, rd );
    float d = 1.0/dot( rd, n );
    float u = d*dot( -q, v2v0 );
    float v = d*dot(  q, v1v0 );
    float t = d*dot( -n, rov0 );
    if( u<0.0 || v<0.0 || (u+v)>1.0 ) t = -1.0;
    return vec3( t, u, v );
}

vec3 castRay(inout vec3 ro, inout vec3 rd){
    vec2 minIt = vec2(99999);
    vec4 col = vec4(0.0);
    vec3 n = vec3(0.0);
    vec3 inter;

    for(int i = 0; i != totalv; i++){
        inter = triIntersect(ro, rd, Mesh[MeshIndices[i].x], Mesh[MeshIndices[i].y], Mesh[MeshIndices[i].z]);
        if(inter.x > 0.0 && inter.x < minIt.x){
            vec3 A = Mesh[MeshIndices[i].y] - Mesh[MeshIndices[i].x];
            vec3 B = Mesh[MeshIndices[i].z] - Mesh[MeshIndices[i].x];
            n = normalize(cross(A, B));
            minIt.x = inter.x;
            col = vec4(n, 0.0);
        }
    }
    
    if(col.a <= 1.0){
        vec3 rand = randomOnSphere();
        vec3 spec = reflect(rd, n);
        vec3 diff = normalize(rand * dot(rand, n));
        ro += rd * vec3(minIt.x - 0.001);
        rd = mix(spec, diff, col.a);
    }
    if(col.a < 0.0){
        ro += rd * vec3(minIt.y - 0.001);
        rd = refract(rd, n, 1.0 / (1.0 - col.a));
    }
    return vec3(col.r, col.g, col.b);
}

vec3 RayTracing(vec3 ro, vec3 rd){
    vec3 col = vec3(1.0);
    for(int i = 0; i != Settings.y;i++){
        vec3 second = castRay(ro, rd);
        col *= second;
    }
    return col;
}

void main(){
    vec2 uvx = (vec2(uv.x, uv.y)-vec2(0.5))*vec2(resolution)/vec2(resolution.y, resolution.x);
    vec3 rayOrigin = vec3(position);
    vec3 rayDirection = normalize(vec3(1.0, uvx));
    rayDirection.zx *= rot(rotation.y);
	rayDirection.xy *= rot(rotation.x);
    vec3 col = vec3(0);
    for(int i = 0; i!= Settings.x; i++){
        col += RayTracing(rayOrigin, rayDirection);
    }
    FragColor = vec4(col, 1.0);
}