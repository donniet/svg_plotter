
function normalize_stroke(stroke, animation_bounds, scale = 1.)
{
  let s = stroke.map((p) => {
    return { 
      x: p.x * scale / animation_bounds.x, 
      y: p.y * scale / animation_bounds.y, 
      t: p.t,
      is_move: p.t == 0
    };
  });

  let ct = 0.;
  for(let i = 0; i < s.length; i++)
  {
    ct += s[i].t;
    s[i].t = ct;
  }
  for(let i = 0; i < s.length; i++)
    s[i].t = s[i].t / ct;
  
  return s;
}

function scale_stroke(stroke, scale)
{
  return stroke.map((p) => {
    return {
      x: p.x * scale,
      y: p.y * scale,
      t: p.t
    };
  });
}

function render_stroke_position(ctx, stroke, brush_diameter, animation_bounds)
{
  let normalized = normalize_stroke(stroke, animation_bounds);

  let pen = {x: 0, y: 0, t: 0};
  let time = 0.;

  ctx.lineWidth = brush_diameter;
  ctx.lineCap = "round";
  ctx.lineJoin = "round";

  ctx.globalCompositionOperation = "destination-out";
  
  for(let i = 1; i < stroke.length; i++)
  {
    let n = normalized[i];
    let s = stroke[i];

    if(n.is_move)
      continue;

    let n0 = normalized[i-1];
    let s0 = stroke[i-1];

    let ds = { x: s.x - s0.x, y: s.y - s0.y };
    let l = Math.sqrt(ds.x * ds.x + ds.y * ds.y);
    ds.x /= l;
    ds.y /= l;
    ds.x *= brush_diameter/2;
    ds.y *= brush_diameter/2;

    let grad = ctx.createLinearGradient(s0.x - ds.x, s0.y - ds.y, s.x + ds.x, s.y + ds.y);
    grad.addColorStop(0, `rgba(${n0.x * 255}, ${n0.y * 255}, 0, ${n0.t})`);
    grad.addColorStop(1, `rgba(${ n.x * 255}, ${ n.y * 255}, 0, ${ n.t})`);
  
    ctx.strokeStyle = grad;
    // ctx.strokeStyle = `rgb(${n.x * 256}, ${n.y * 256}, 0)`;
    ctx.beginPath();
    ctx.moveTo(s0.x, s0.y);
    ctx.lineTo(s.x, s.y);
    ctx.stroke();
  }
}

// returns two sampler objects, stroke_position and stroke_direction
function render_stroke_samplers(stroke, brush_diameter, animation_bounds)
{
  return with_canvas(animation_bounds, 
  (canvas) => {
    const ctx = canvas.getContext("2d");

    render_stroke_position(ctx, stroke, brush_diameter, animation_bounds);
  });
}

function test_rendering()
{
  const canvas = document.createElement("canvas");
  canvas.width = 100;
  canvas.height = 100;

  const ctx = canvas.getContext("2d");

  let s = scale_stroke(test_stroke, 10);

  render_stroke_position(ctx, s, 7, {x: 100, y: 100, t: 1000 /* ms */ });

  canvas.toBlob((blob) => {
    const newImg = document.createElement("img");
    const url = URL.createObjectURL(blob);
  
    newImg.onload = () => {
      // no longer need to read the blob so it's revoked
      URL.revokeObjectURL(url);
    };
  
    newImg.src = url;
    document.body.appendChild(newImg);
  });
}
