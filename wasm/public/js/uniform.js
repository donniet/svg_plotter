// import gl_uniform_method_for, gl_uniform_is_matrix from "utility"


function Uniform(name, type, data = null, location = null)
{
    this.name = name;
    this.type = type;
    this.gl_setter = gl_uniform_method_for(type);
    this.data = data;
    this.location = location;
    this.is_matrix = gl_uniform_is_matrix(type);
}

Uniform.prototype.load_from = function(shader)
{
    this.location = shader.uniform_location(this.name);
}

Uniform.prototype.set = function(gl, value, transpose) 
{
    if(this.location === null)
    {
        if(!this.__warned__)
        {
            // this warning keeps printing because this object goes out of scope
            // but I'd like to keep the warning in
            console.log(`uniform ${this.name} not found in program`);
            this.__warned__ = true;
        }
        return;
    }

    if(typeof value === "undefined" && this.data == null)
    {
        console.log(`no value set for uniform "${this.name}"`);
        return;
    }

    if(typeof value === "undefined")
        if(this.is_matrix)
            gl[this.gl_setter](this.location, transpose || false, this.data);
        else
            gl[this.gl_setter](this.location, this.data);
    else if(typeof transpose === "undefined")
        if(this.is_matrix)
            gl[this.gl_setter](this.location, transpose || false, value);
        else
            gl[this.gl_setter](this.location, value);
    else
        gl[this.gl_setter](this.location, transpose, value);
}