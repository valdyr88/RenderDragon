
#define MATERIAL_MAX_DECALS 8
#define RENDERER_MAX_LIGHTS 4

#define FLOAT_EQUALS(a,b,e) ((abs(a - b) - e < 0.0))
#define PI 3.1415926536
#define PI_DIV_2 1.570796327
#define PI_DIV_3 1.047197551196
#define PI_DIV_4 0.7853982
#define LinearizeDepth(x,n,f) ((2 * n) / (f + n - x * (f - n)))
#define LinearizeDepthAndSubtract(b,a,n,f) (LinearizeDepth(b,n,f) - LinearizeDepth(a,n,f))
#define LinearizeDepth2(x,n,f) ((2 * n) / (f + lerp(n,f,-x)))
#define LinearizeDepthAndSubtract2(b,a,n,f) (LinearizeDepth2(b,n,f) - LinearizeDepth2(a,n,f))
#define LinearizeZ(z,Near,Far) ( 2*Far*Near / ( Far + Near - ( Far - Near ) * ( 2*z - 1 ) ) )
