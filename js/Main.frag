#version 300 es

precision highp float;
out vec4 fragColor;
in vec2 uv;

uniform vec2 resolution;

uniform vec2 rotation;

uniform vec3 position;

uniform vec4 random;

uvec4 R_STATE;

uint TausStep(uint z, int S1, int S2, int S3, uint M)
{
	uint b = (((z << S1) ^ z) >> S2);
	return (((z & M) << S3) ^ b);	
}

uint LCGStep(uint z, uint A, uint C)
{
	return (A * z + C);	
}

vec2 hash22(vec2 p)
{
	p += random.x;
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

float randompp()
{
	R_STATE.x = TausStep(R_STATE.x, 13, 19, 12, uint(4294967294));
	R_STATE.y = TausStep(R_STATE.y, 2, 25, 4, uint(4294967288));
	R_STATE.z = TausStep(R_STATE.z, 3, 11, 17, uint(4294967280));
	R_STATE.w = LCGStep(R_STATE.w, uint(1664525), uint(1013904223));
	return 2.3283064365387e-10 * float((R_STATE.x ^ R_STATE.y ^ R_STATE.z ^ R_STATE.w));
}

vec3 randomOnSphere() {
	vec3 rand = vec3(randompp(), randompp(), randompp());
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

vec2 sphIntersect( in vec3 ro, in vec3 rd, float ra ){
    float b = dot(ro, rd);
    float c = dot(ro, ro) - ra*ra;
    float h = b * b - c;
    if(h < 0.0) return vec2(-1.0);
    h - sqrt(h);
    return vec2(-b-h, -b+h);
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

float plaIntersect(in vec3 ro, in vec3 rd, in vec4 p) {
	return -(dot(ro, p.xyz) + p.w) / dot(rd, p.xyz);
}

vec3 triIntersect( in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2 ){
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
    vec2 minIt = vec2(10000);
    vec4 col = vec4(0.0);
    vec3 n;
    vec3 inter = triIntersect(ro, rd, vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
    if(inter.x > 0.0){
        col = vec4(1.0, 1.0, 1.0, -1.0);
    }
    if(col.a <= 1.0){
        vec3 rand = randomOnSphere();
        vec3 spec = reflect(rd, n);
        vec3 diff = normalize(rand * dot(rand, n));
        ro += rd * vec3(minIt.x - 0.001);
        rd = mix(spec, diff, col.a);
    }
    if(col.a >= 3.0){
        ro += rd * vec3(minIt.y - 0.001);
        rd = refract(rd, n, 1.0/1.2);
    }
    return vec3(col.r, col.g, col.b);
}

vec3 RayTracing(vec3 ro, vec3 rd){
    vec3 col = vec3(1.0f);
    for(int i = 0; i != 1;i++){
        vec3 second = castRay(ro, rd);
        col *= second;
    }
    return col;
}

void main(){

    vec2 uvx = (vec2(uv.x, uv.y)-vec2(0.5))*vec2(resolution)/vec2(resolution.y, resolution.x);
    vec2 locuv = gl_FragCoord.xy / resolution;
	vec2 uvRes = hash22(locuv + 1.0) * resolution + resolution;
	R_STATE.x = uint(random.x + uvRes.x);
	R_STATE.y = uint(random.y + uvRes.y);
	R_STATE.z = uint(random.z + uvRes.x);
	R_STATE.w = uint(random.w + uvRes.y);

    vec3 rayOrigin = vec3(position);
    vec3 rayDirection = normalize(vec3(1.0, uvx));
    rayDirection.zx *= rot(rotation.y);
	rayDirection.xy *= rot(rotation.x);
    vec3 col = vec3(0);
    for(int i = 0; i!= 1; i++){
        col += RayTracing(rayOrigin, rayDirection);
    }
    fragColor = vec4(col, 1.0);
}