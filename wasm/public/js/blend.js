// import float32rray_from_wasm from "utility"


function Blend(app, blend_ptr)
{
    this.app = app;
    this.blend_ptr = blend_ptr;
    const exports = app.exports;

    if(!this.blend_ptr)
        return;

    const rgba = exports.blend_color(blend_ptr);

    this.color = float32array_from_wasm(exports, rgba, 4);

    this.equations = [
        exports.blend_equation_rgb(blend_ptr),
        exports.blend_equation_alpha(blend_ptr),
    ];

    this.funcs = [
        exports.blend_source_rgb(blend_ptr),
        exports.blend_destination_rgb(blend_ptr),
        exports.blend_source_alpha(blend_ptr),
        exports.blend_destination_alpha(blend_ptr),
    ];
}

Blend.prototype.apply = function(gl)
{
    if(!this.blend_ptr) 
    {
        gl.disable(gl.BLEND);
        return;
    }

    gl.enable(gl.BLEND);
    gl.blendColor(...this.color);
    gl.blendEquationSeparate(...this.equations);
    gl.blendFuncSeparate(...this.funcs);
}

