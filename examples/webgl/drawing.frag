#version 300 es

precision highp float;

in vec2 uv;
in vec2 pos;
in vec2 brush;


uniform vec3 u_brush_color;
uniform float u_brush_size;
uniform vec2 u_range;
uniform vec2 u_resolution;
uniform vec4 u_uv_range;

out vec4 fragColor;

// calculate the nearest point on the centerline of the stoke
// the centerline has uv coordinates of (s, 0)
// where s is the arclength
// vec2 centerline(vec2 position, vec2 uv, float s)
// {
//     // pos = A * uv + b

// }


float smoothf(float x)
{
    return x*x*x*(x*(x*6. - 15.) + 10.);
}

float dtoa(float d, float amount)
{
    float x = clamp(d, 1.0/amount, 1.0) * amount;
    return clamp(1.0 / x, 0.,1.);
}

// hash & simplex noise from https://www.shadertoy.com/view/Msf3WH
vec2 hash( vec2 p )
{
	p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

// returns -.5 to 1.5. i think.
float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2 i = floor( p + (p.x+p.y)*K1 );
	
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );	
}


float noise01(vec2 p)
{
    return clamp((noise(p)+.5)*.5, 0.,1.);
}

vec4 brush_stroke_color(vec4 input_color)
{
    float d = length(pos - brush) / 50.;

    // float d = length(uv - vec2(arclength / 50., 0));
    // float d = uv_range.y - uv.y;
    // float d = 1.;

    if(uv.y > u_uv_range[3])
        d = length(uv.xy - vec2(0, u_uv_range[3]));

    if(uv.y < u_uv_range[1])
        d = length(uv.xy - vec2(0,u_uv_range[1]));

    if(d > 0.5)
        return vec4(u_brush_color.xyz, 0.);
    

    // float boundary = dtoa(d, 0.3);
    float tex = 0.
        + noise01(uv.xy * vec2(min(u_resolution.y, u_resolution.x) * 0.3, 0.5))
        + noise01(uv.xy * vec2(79., 1.))
        + noise01(uv.xy * vec2(44., 1.))
        ;

    tex *= 0.333 * abs(uv.x);
    tex = max(0.008, tex);

    float alpha = pow(tex, max(0., d));

    const float alpha_boost = 1.19;

    alpha = alpha_boost * max(0., alpha - pow(2. * d, 0.5));
    alpha = smoothf(alpha);


    
//     return mix(input_color, brush_color, alpha);
    return vec4(u_brush_color.xyz, alpha);
}


void main()
{
    // float dummy = u_range.x + u_brush_color.x + u_brush_size;

    // fragColor = vec4(1,0,dummy,1);
    // fragColor = vec4(0,1,0,1);
    vec4 col = brush_stroke_color(vec4(1., 1., 1., 1.));

    // fragColor = vec4(1., col.a, 0., 1.);
    fragColor = col;
}