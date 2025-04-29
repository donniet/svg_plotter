
async function init()
{
    const canvas = document.querySelector("canvas");
    const gl = canvas.getContext(WEBGL_CONTEXT_NAME, {
        antialias: false,
    });
    
    const imports = {
        log: function(msg) { console.log(msg); }
    };

    // TODO: lookup wasm by time
    // const { instance } = await WebAssembly.instantiateStreaming(
    //     fetch(DEFAULT_DRAWING), imports
    // );

    const solid_wasm = await WebAssembly.instantiateStreaming(
        fetch(DEFAULT_SOLID), imports
    );

    const lut = new Image();
    lut.src = MIXING_LOOKUP_TABLE;
    
    await new Promise(function (resolve) {
        lut.onload = resolve;
    });

    // compile and link our shader program
    const blend_program = await create_program_from_files(gl, BLEND_VERTEX_SHADER, BLEND_FRAGMENT_SHADER);
    // const blend_program = create_program_from_scripts(gl, [blend_vertex_shader, blend_fragment_shader]);


    if(RENDER_DRAWING)
    {
        // parse the wasm instance into mesh data
        const mesh = new Mesh(instance);

        // get the elements containing our vertex and fragment shaders
        // const vertex_shader = document.getElementById("vertex_shader");
        // const fragment_shader = document.getElementById("fragment_shader");

        // compile and link our shader program
        const program = await create_program_from_files(gl, DRAWING_VERTEX_SHADER, DRAWING_FRAGMENT_SHADER);
        // const program = create_program_from_scripts(gl, [vertex_shader, fragment_shader]);

        
        // get the elements containing our vertex and fragment shaders
        // const blend_vertex_shader = document.getElementById("blend_vertex_shader");
        // const blend_fragment_shader = document.getElementById("blend_fragment_shader");

        
        // setup our drawing class
        const drawing = new Drawing(gl, program, blend_program,
            mesh.vertex_data(), mesh.stride(), 
            mesh.attributes(), mesh.uniforms(), mesh.layers(),
            mesh.drawing_size(), mesh.draw_time(), new Float32Array([.5, 1.0, 1.0, 0.])/* mesh.clear_color() */
        );

        // set our color blending lookup table
        drawing.set_mixbox_lut(gl, lut);

        // begin the animation
     
        drawing.continue_animate(gl);
    }

    if(RENDER_SOLIDS)
    {
        const solids = new Solids(solid_wasm.instance);

        const solid_vertex_source = await fetch_text(SOLID_VERTEX_SHADER);
        const solid_fragment_source = await fetch_text(SOLID_FRAGMENT_SHADER);

        const solid_renderer = new SolidsRenderer(gl, [solid_vertex_source, solid_fragment_source], solids);

        solid_renderer.continue_animate(gl);
    }
}   
