
function Blender(gl, blend_program, clear_color, texture_unit, combined_unit)
{
    this.program = blend_program;
    this.texture_unit = texture_unit || 0;
    this.combined_texture_unit = combined_unit || 1;

    this.position_enable = enable_attribute.bind(this, 
        gl.getAttribLocation(this.program, "a_position"), 16, 2, 0);

    this.uv_enable = enable_attribute.bind(this, 
        gl.getAttribLocation(this.program, "a_uv"),       16, 2, 8);

    const uniform_texture_location = gl.getUniformLocation(this.program, "u_texture");
    this.set_texture = function(texture) {
        gl.activeTexture(gl.TEXTURE0 + this.texture_unit);
        gl.bindTexture(gl.TEXTURE_2D, texture);

        gl.uniform1i(uniform_texture_location, this.texture_unit);
    }.bind(this);

    const uniform_combined_location = gl.getUniformLocation(this.program, "u_combined");
    this.set_combined = function(texture) {
        gl.activeTexture(gl.TEXTURE0 + this.combined_texture_unit);
        gl.bindTexture(gl.TEXTURE_2D, texture);

        gl.uniform1i(uniform_combined_location, this.combined_texture_unit);
    }.bind(this);
}

Blender.prototype.draw = function(gl, texture, combined)
{
    this.position_enable(gl);
    this.uv_enable(gl);
    this.set_texture(texture);
    this.set_combined(combined);

    // gl.enable(gl.BLEND);
    // gl.blendFuncSeparate(gl.DST_ALPHA, gl.SRC_ALPHA, gl.ONE, gl.ONE);
    // gl.blendEquationSeparate(gl.MAX, gl.MAX);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}
