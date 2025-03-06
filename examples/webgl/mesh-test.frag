#version 300 es

precision highp float;

in vec2 uv;
in vec2 pos;
in float arclength;
in vec2 brush;


uniform vec4 brush_color;
uniform vec2 stroke_range;
uniform vec2 resolution;

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
    // float d = length(pos - brush);

    float d = length(uv - vec2(arclength / 50., 0));
    

    // float boundary = dtoa(d, 0.3);
    float tex = 0.
        + noise01(uv.yx * vec2(min(resolution.y, resolution.x) * 0.2, 1.))
        + noise01(uv.yx * vec2(79., 1.))
        + noise01(uv.yx * vec2(14., 1.))
        ;

    tex *= 0.333 * abs(uv.y);
    tex = max(0.008, tex);

    float alpha = pow(tex, max(0., d));

    const float alpha_boost = 1.09;

    alpha = alpha_boost * max(0., alpha - pow(2. * d, 0.5));
    alpha = smoothf(alpha);

    
    return mix(input_color, brush_color, alpha);
}

void main()
{
    // uv.x == arclength of brushstroke
    // uv.y == distance from centerline
    // brush diameter is 1

    // vec2 rel = vec2(arclength, 0) - uv;

    // float d2 = dot(rel, rel);

    // fragColor.w = 0.25 - d2;
    // fragColor.xyz = brush_color.xyz;

    fragColor = brush_stroke_color(vec4(0., 0., 0., 0.));

    // fragColor.xyzw = (0.25 - d2) * brush_color.xyzw;

    // fragColor = vec4( uv.xy,0, 1);
}