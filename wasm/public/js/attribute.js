// import bytes_from_element_type from "utility"


function Attribute(name, offset, stride, normalized)
{
    this.name = name;
    this.offset = offset;
    this.stride = stride;
    this.normalized = normalized;
    this.shader = null;
    this.definition = null;
}
Attribute.prototype.load_from = function(shader)
{
    this.shader = shader;
    this.definition = shader.attribute_definition(this.name);
};
Attribute.prototype.enable = function(gl)
{
    if(this.definition === null)
        throw new Error(`attempt to enable attribute ${this.name} without definition`);

    this.definition.enable(gl, this.offset, this.stride, this.normalized);
};

function AttributeDef(name, size, type)
{
    this.name = name;
    this.size = size;
    this.type = type;
    this.location = null;
}
AttributeDef.prototype.enable = function(gl, offset = 0, stride = 0, normalized = false)
{
    if(this.location == null)
    {
        if(!this.__warned__)
        {
            console.log(`attribute "${this.name}" not found in program.`);
            this.__warned__ = true;
        }
        return;
    }

    // assume this is the only attribute if stride is not set
    if(stride === 0)
        stride = this.size * bytes_from_element_type(gl, this.type);

    gl.enableVertexAttribArray(this.location);
    gl.vertexAttribPointer(this.location, this.size, this.type, 
        normalized, stride, offset);
};

