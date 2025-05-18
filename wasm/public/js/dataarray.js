
function DataArray(name, data, type, target)
{
    this.name = name;
    this.data = data;
    this.type = type;
    this.target = target;
    this.buffer = null;
}
DataArray.prototype.load = function(gl, usage)
{
    if(typeof usage === "undefined")
        usage = gl.STATIC_DRAW;

    this.buffer = gl.createBuffer();
    gl.bindBuffer(this.target, this.buffer);
    gl.bufferData(this.target, this.data, usage);
}
DataArray.prototype.bind = function(gl)
{
    gl.bindBuffer(this.target, this.buffer);
}

