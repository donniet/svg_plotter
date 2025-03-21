#version 300 es

precision highp float;

in vec2 uv;
in vec2 pos;
in vec2 brush;
in float arclength;


uniform float u_stroke_length;
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

float rand(float n)
{
    return fract(sin(n) * 43758.5453123);
}
float rand(vec2 n) 
{ 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(float p)
{
	float fl = floor(p);
    float fc = fract(p);

	return mix(rand(fl), rand(fl + 1.0), fc);
}
	
float noise(vec2 n) {
    const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), 
         f = smoothstep(vec2(0.0), vec2(1.0), fract(n));

	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

// // returns -.5 to 1.5. i think.
// float noise( in vec2 p )
// {
//     const float K1 = 0.366025404; // (sqrt(3)-1)/2;
//     const float K2 = 0.211324865; // (3-sqrt(3))/6;

// 	vec2 i = floor( p + (p.x+p.y)*K1 );
	
//     vec2 a = p - i + (i.x+i.y)*K2;
//     vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
//     vec2 b = a - o + K2;
// 	vec2 c = a - 1.0 + 2.0*K2;

//     vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

// 	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

//     return dot( n, vec3(70.0) );	
// }


// float noise01(vec2 p)
// {
//     return clamp((noise(p)+.5)*.5, 0.,1.);
// }

float distance_to_alpha(float d, float a)
{
    return clamp(1. / clamp( d, 1. / a, 1.) / a, 0., 1.);
}

/**
 * @param {vec2} uv parameters of the line: 
 *   x == distance from centerline
 *   y == distance along line
 */
vec4 brush_stroke_color(vec2 uv, vec4 input_color)
{
    // return input_color;

    // float d = length(pos - brush) / u_brush_size * 2.;

    int loc = 0;

    float du = abs(uv.x);
    float dv = 0.;

    if(uv.y < u_uv_range[1])
    {
        dv = u_uv_range[1] - uv.y;
        loc = -1; // beginning
    }
    else if(uv.y > u_uv_range[3])
    {
        dv = uv.y - u_uv_range[3];
        loc = 1; // end
    }
    float d = length(vec2(du,dv));

    // return vec4(u_brush_color.xyz, clamp(2. - d, 0., 1.));
    // return vec4(u_brush_color.xyz, 1.);
    // return vec4(1., 0., 0., 1.-d);


    // float d = length(uv - vec2(arclength / 50., 0));
    // float d = uv_range.y - uv.y;
    // float d = 1.;

    // if(uv.y > u_uv_range[3])
    //     d = length(uv.xy - vec2(0, u_uv_range[3]));

    // if(uv.y < u_uv_range[1])
    //     d = length(uv.xy - vec2(0,u_uv_range[1]));

    if(d > 1.0)
        return vec4(u_brush_color.xyz, 0.);

    // return vec4(1., 0., 0., 1.-d);

    // float x = noise(7.);

    // float boundary = distance_to_alpha(d, 0.3);
    float tex = 0.3333 * (0. +
        + noise(uv.xy * vec2(min(u_resolution.x, u_resolution.y) * 0.1, 0.5))
        + noise(uv.xy * vec2(100., 1.))
        + noise(uv.xy * vec2(44., 1.))
    );

    // tex *= 0.333 * abs(uv.x);
    tex = max(0.008, tex);
    // return vec4(u_brush_color.xyz, tex);

    float alpha = pow(tex, max(0., d ));

    const float alpha_boost = 1.59;

    alpha = alpha_boost * max(0., alpha - pow(d, 0.5));
    alpha = smoothf(alpha);


    
    // return mix(input_color, vec4(u_brush_color, 1), alpha);
    return vec4(u_brush_color.xyz, alpha);
}


void main()
{
    // float dummy = u_range.x + u_brush_color.x + u_brush_size;

    // fragColor = vec4(1,0,dummy,1);
    // fragColor = vec4(0,1,0,1);

    vec4 col = brush_stroke_color(uv, vec4(1., 1., 1., 1.));
    fragColor = col;
    // fragColor = vec4(1., col.a, 0., 1.);
    // fragColor = col;
    // fragColor = vec4(u_brush_color.xyz, 1.);
    // fragColor = vec4(0.03515625, 0.1796875, 0.1640625, 1.);
}