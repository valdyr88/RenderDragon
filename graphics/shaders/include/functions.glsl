
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define matrix mat4

#define inline

#define frac fract

#define mul(x,y) (x * y)
#define sincos(a,s,c) { s = sin(a); c = cos(a); }

//================================================================================================================

inline float4 tofloat4(float3 a, float b){
	return float4(a.x,a.y,a.z,b);
}
inline float4 tofloat4(float a, float3 b){
	return float4(a,b.x,b.y,b.z);
}
inline float4 tofloat4(float2 a, float2 b){
	return float4(a.x,a.y,b.x,b.y);
}
inline float4 tofloat4(float a){
	return float4(a,a,a,a);
}

inline float3 tofloat3(float4 a){
	return float3(a.x,a.y,a.z);
}
inline float3 tofloat3(float2 a, float b){
	return float3(a.x,a.y,b);
}
inline float3 tofloat3(float a, float2 b){
	return float3(a,b.x,b.y);
}
inline float3 tofloat3(float a){
	return float3(a,a,a);
}

inline float2 tofloat2(float4 a){
	return float2(a.x,a.y);
}
inline float2 tofloat2(float3 a){
	return float2(a.x,a.y);
}
inline float2 tofloat2(float a){
	return float2(a,a);
}

//================================================================================================================

inline float rand(in float2 co)
{
	return 2*fract(abs(sin(dot(co.xy , float2(12.9898,78.233))) * 43758.5453))-1;
}
inline float2 randf2(in float2 co){
	float f1 = fract(abs(sin(dot(co.xy , float2(12.9898,78.233))) * 43758.5453));
	float f2 = fract(abs(cos(dot(co.yx , float2(12.9898,78.233))) * 43758.5453));
	return 2*float2(f1,f2)-1;
}
inline float3 randf3(in float2 co){
	float dot1 = dot(co.xy , float2(12.9898,78.233));
	float dot2 = dot(co.yx , float2(2.7182817,42.76908));
	float f1 = fract(abs(cos(dot1) * 43758.5453));
	float f2 = fract(abs(sin(dot1) * 43758.5453));
	float f3 = fract(abs(cos(dot2) * 43758.5453));
	return 2*float3(f1,f2,f3)-1;
}
inline float4 randf4(in float2 co){
	float dot1 = dot(co.xy , float2(12.9898,78.233));
	float dot2 = dot(co.yx , float2(2.7182817,42.76908));
	float f1 = fract(abs(sin(dot1) * 43758.5453));
	float f2 = fract(abs(cos(dot1) * 43758.5453));
	float f3 = fract(abs(sin(dot2) * 43758.5453));
	float f4 = fract(abs(cos(dot2) * 43758.5453));
	return 2*float4(f1,f2,f3,f4)-1;
}

//================================================================================================================

inline float gauss(in float b, in float4 x){
	return 1.0 / pow(b, dot(x,x));
}
inline float gauss(in float b, in float3 x){
	return 1.0 / pow(b, dot(x,x));
}
inline float gauss(in float b, in float2 x){
	return 1.0 / pow(b, dot(x,x));
}
inline float gauss(in float b, in float x){
	return 1.0 / pow(b,x*x);
}

//================================================================================================================

float hsl_lightness(float3 color){
	return (min(min(color.x,color.y),color.z) + max(max(color.x,color.y),color.z)) / 2;
}
//================================================================================================================

inline float desaturate(float3 color)
{
	float luminance = dot(color,float3(0.299,0.587,0.114)); 
	return luminance;
}

//================================================================================================================

inline matrix MatrixFromEulerAngles(float3 Angles){
	matrix m;
	float3 s, c;
	sincos(Angles,s,c);
	m[0][0]=c.y*c.z;	m[1][0]=c.z*s.x*s.y+c.x*s.z;	m[2][0]=-c.x*c.z*s.y+s.x*s.z;	m[3][0] = 0;
	m[0][1]=-c.y*s.z;	m[1][1]=c.x*c.z-s.x*s.y*s.z;	m[2][1]=c.z*s.x+c.x*s.y*s.z;	m[3][1] = 0;
	m[0][2]=s.y;		m[1][2]=-c.y*s.x;				m[2][2]=c.x*c.y;				m[3][2] = 0;
	m[0][3] = 0;		m[1][3] = 0;					m[2][3] = 0;					m[3][3] = 1;
	return m;
}
inline matrix getRotationMatrix(in float3 RotationUp, in float3 RotationForward){
	matrix m;
	float3 RotationRight = cross(RotationUp,RotationForward);
	m[0][0]=RotationRight.x;	m[1][0]=RotationUp.x;	m[2][0]=RotationForward.x;	m[3][0] = 0;
	m[0][1]=RotationRight.y;	m[1][1]=RotationUp.y;	m[2][1]=RotationForward.y;	m[3][1] = 0;
	m[0][2]=RotationRight.z;	m[1][2]=RotationUp.z;	m[2][2]=RotationForward.z;	m[3][2] = 0;
	m[0][3] = 0;				m[1][3] = 0;			m[2][3] = 0;				m[3][3] = 1;
	return m;
}
inline void ClearTranslation(inout matrix M){
	M[3][0] = 0; M[3][1] = 0; M[3][2] = 0; M[3][3] = 1;
}
inline matrix ZeroMatrix(){
	matrix M;
	for(int x = 0; x < 4; ++x) for(int y = 0; y < 4; ++y) M[x][y] = 0.0f;
	return M;
}
inline matrix IdentityMatrix(){
	matrix M = ZeroMatrix();
	for(int x = 0; x < 4; ++x) M[x][x] = 1;
	return M;
}

inline void getBasisVectors(in matrix M, out float3 r, out float3 u, out float3 f){
	r.x = M[0][0]; r.y = M[0][1]; r.z = M[0][2];
	u.x = M[1][0]; u.y = M[1][1]; u.z = M[1][2];
	f.x = M[2][0]; f.y = M[2][1]; f.z = M[2][2];
}

inline float3 getTranslation(in matrix M){
	return float3( M[3][0], M[3][1], M[3][2] );
}

inline matrix getRotationMatrix(in matrix M){
	float3 r, u, f;
	
	r = M[0].xyz; r = normalize(r);
	u = M[1].xyz; u = normalize(u);
	f = M[2].xyz; f = normalize(f);
	
	matrix m = ZeroMatrix();
	m[0].xyz = r;
	m[1].xyz = u;
	m[2].xyz = f;
	m[2][2] = 1.0f;
	
	return m;
}

inline matrix getInverseRotationMatrix(in matrix M){
	matrix m = getRotationMatrix(M);
	
	m[0].xyz = -m[0].xyz;
	m[1].xyz = -m[1].xyz;
	m[2].xyz = -m[2].xyz;
	
	return m;
}

//prima World matricu
matrix getTransformForNormalsMatrix(in matrix W){
/*

- world kopirati,
- clearati translaciju
- izvući vektore iz stupaca
- izracunati kvadratni length vektora
- konstruirati kvadratnu scale matricu
- vratiti umnozak kvadratnog scalea i kopiranog worlda

*/
	
//clearanje translacije
	ClearTranslation(W);
	
//kvadratni koeficijenti scalea
	float3 s2; float3 r, u, f;
	getBasisVectors(W, r, u, f);
	s2.x = dot(r, r);
	s2.y = dot(u, u);
	s2.z = dot(f, f);
	
//inverz koeficijenta
	s2 = 1.0f / s2;
	
//scale matrica
	matrix S2 = IdentityMatrix();
	S2[0][0] = s2.x; S2[1][1] = s2.y; S2[2][2] = s2.z;
	
//vratiti umnozak kvadratnog scalea i kopiranog worlda
	return mul(S2,W);
}

//================================================================================================================

inline float4 baryc(float4 a, float4 b, float4 c, float3 t){
	float4 rtn = tofloat4(0.0f);
	rtn = t.x * a + t.y * b + t.z * c;
	return rtn;
}
inline float3 baryc(float3 a, float3 b, float3 c, float3 t){
	float3 rtn = tofloat3(0.0f);
	rtn = t.x * a + t.y * b + t.z * c;
	return rtn;
}
inline float2 baryc(float2 a, float2 b, float2 c, float3 t){
	float2 rtn = tofloat2(0.0f);
	rtn = t.x * a + t.y * b + t.z * c;
	return rtn;
}
inline float baryc(float a, float b, float c, float3 t){
	float rtn = 0.0f;
	rtn = t.x * a + t.y * b + t.z * c;
	return rtn;
}

//================================================================================================================

inline float saturate2(float x, float l){
	float ix = 1.0f - l*x;
	return x * ix*ix;
}
inline float3 saturate2(float3 x, float l){
	float s = saturate2(desaturate(x),l);
	return s * x;
}
inline float4 saturate2(float4 x, float l){
	float s = saturate2(desaturate(x.xyz),l);
	return float4(s*x.xyz, x.a);
}

//================================================================================================================

inline float3 halfvec(in float3 a, in float3 b){
	return -normalize(0.5f*(a - b) - a);
}
inline float4 reflectvec(in float3 ray, in float3 normal){
	float4 v = float4(0,0,0, dot(ray,normal));
	v.xyz = ray - 2*v.a*normal;//minus je jer je dot(ray,normal) negativan, tj ray i normal su suprotno orjentirani
	return v;
}

//================================================================================================================

inline float3 fresnel(in float3 f0, in float dotProduct){
	//return lerp(f0, tofloat3(1.0f), pow(1.0001f - dotProduct, 5.0f));
	//float m = 1.0f - dotProduct; float m2 = m*m; float m5 = m2*m2*m;
	return f0 + (1.0f - f0)*pow(1.0f - dotProduct, 5.0f);
}
inline float3 fresnel(in float3 f0, in float3 view, in float3 ray){
	return fresnel(f0, dot(halfvec(view,ray), view));
}

//================================================================================================================

inline void excg(inout float a, inout float b){
	float tmp = a; a = b; b = tmp;
}
inline void excg(inout float2 a, inout float2 b){
	float2 tmp = a; a = b; b = tmp;
}
inline void excg(inout float3 a, inout float3 b){
	float3 tmp = a; a = b; b = tmp;
}
inline void excg(inout float4 a, inout float4 b){
	float4 tmp = a; a = b; b = tmp;
}

//================================================================================================================




















































