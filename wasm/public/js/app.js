
function App()
{
    this.drinks = [];
    this.locations = [];

    // setup our canvas object
    this.canvas = document.getElementsByTagName('canvas')[0];
    this.gl = this.canvas.getContext(CONTEXT_NAME, {
        antialias: false,
    });

    this.module_imports = {
        log: function(msg) { console.log(msg); }
    };
}

App.prototype.fetch_shader = async function(shader_path)
{
    return fetch_text(SHADER_PREFIX + shader_path);
}


App.prototype.draw = function(range)
{
    let resized = this.resize_canvas(this.gl, 
        window.innerWidth,
        window.innerHeight);

    this.drinks.forEach(drink => {
        if(resized)
            drink.viewport(...this.viewport_dimensions);

        drink.draw(this.gl, range);
    });
}

App.prototype.continue_animate = function(time)
{
    if(typeof time != "undefined")
        this.draw([0, time]);

    requestAnimationFrame(time => this.continue_animate(time));
}

App.prototype.init = async function(mod) 
{
    // load our wasm module
    console.log(`loading module ${mod}...`);
    this.wasm = await WebAssembly.instantiateStreaming(
        fetch(mod), this.module_imports
    );
    const exports = this.wasm.instance.exports;
    this.exports = exports;

    // helper function for extracting strings
    this.string = s => 
        string_from_wasm(exports, s, exports.string_length(s));

    

    this.shaders = new Map();
    const shader_count = exports.shader_count();

    for(let i = 0; i < shader_count; i++)
    {
        const s = exports.shader_by_index(i);
        this.shaders.set(this.string(exports.shader_name(s)), 
            new Shader(this, s));
    }

    console.log(`loading drinks...`);
    const drink_count = exports.drink_count();
    for(let i = 0; i < drink_count; i++)
    {
        const d = exports.drink_by_index(i);
        this.drinks.push(new Drink(this, d));
    }

    console.log(`loading arrays...`);
    this.arrays = new Map();
    const array_count = exports.array_count();
    for(let i = 0; i < array_count; i++)
    {
        const a = exports.array_by_index(i);
        const name = this.string(exports.array_name(a));
        const t = exports.array_type(a);
        const array_type = this.string(exports.array_type_name(t));
        const size = exports.array_type(a);
        const ptr = exports.array_data(a);
        const target = exports.array_target(a);

        // find the constructor for this type of data array
        const con = window[array_type];

        const arr = new DataArray(
            name, new con(exports.memory.buffer, ptr, size), t, target);

        this.arrays.set(name, arr);
    }

    console.log(`setting up canvas...`);
    this.canvas_size = [
        exports.canvas_width(),
        exports.canvas_height(),
    ];
    this.aspect_ratio = this.canvas_size[0] / this.canvas_size[1];

    console.log('fetching shaders...');
    let promises = [];
    this.shaders.forEach(shader => promises.push(shader.load()));

    await Promise.all(promises);

    console.log(`compiling shaders...`);
    this.shaders.forEach(shader => shader.compile(this.gl));

    console.log(`initializing buffers...`);
    this.arrays.forEach(arr => arr.load(this.gl));

    console.log(`loading drawings...`);
    this.drinks.forEach(drink => {
        drink.drawing.uniform(UNIFORM_VIEW, "mat3", view_from(this.canvas_size));
        drink.load(this.gl);
    });
    this.locations.forEach(location => {
        location.drawing.uniform(UNIFORM_VIEW, "mat3", view_from(this.canvas_size));
        location.load(this.gl);
    });
};

App.prototype.uniform_at = function(u)
{
    const exports = this.exports;
    const name = this.string(exports.uniform_name(u));
    const type = this.string(exports.uniform_type(u));
    const data = exports.uniform_data(u);
    const data_size = exports.uniform_data_size(u);

    const array_type = this.string(exports.array_type_name(exports.uniform_gl_type(u)));

    const v = new window[array_type](exports.memory.buffer,
        data, data_size);

    return new Uniform(name, type, v);
}

App.prototype.shader = function(name)
{
    return this.shaders.get(name);
};

App.prototype.data_array = function(name)
{
    return this.arrays.get(name);
};

App.prototype.resize_canvas = function(gl, clientWidth, clientHeight)
{
    if(typeof this.viewport_dimensions === "undefined")
        this.viewport_dimensions = [0, 0, clientWidth, clientHeight];

    if (gl.canvas.width !== clientWidth || gl.canvas.height !== clientHeight) 
    {
        gl.canvas.width  = clientWidth;
        gl.canvas.height = clientHeight;

        let width_by_height = gl.canvas.height * this.aspect_ratio;
        let height_by_width = gl.canvas.width / this.aspect_ratio;

        if(gl.canvas.height > height_by_width)
            this.viewport_dimensions = [
                0, (gl.canvas.height - height_by_width) / 2., gl.canvas.width, height_by_width 
            ];
        else
            this.viewport_dimensions = [
                (gl.canvas.width - width_by_height) / 2., 0, width_by_height, gl.canvas.height
            ];
        
        return true;
    }

    return false;
};

App.on_body_load = async function(mod = IMPORT) 
{
    const app = new App();
    await app.init(mod);
    app.continue_animate();
}

