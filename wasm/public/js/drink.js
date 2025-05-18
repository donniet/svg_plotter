
function Drink(app, drink_ptr)
{
    this.app = app;
    const exports = app.exports;
    this.drink_ptr = drink_ptr;
    this.exports = exports;

    this.name = app.string(exports.drink_name(drink_ptr));
    this.drawing = new Drawing(app, exports.drink_drawing(drink_ptr));
    this.position = [0,0];
}
Drink.prototype.load = function(gl)
{
    this.drawing.load(gl);
};
Drink.prototype.viewport = function(x0, y0, x1, y1)
{
    console.log(`drink.viewport(${x0},${y0},${x1},${y1})`);
};
Drink.prototype.draw = function(gl, range)
{
    this.drawing.uniform(UNIFORM_DRINK_POSITION, "vec2", this.position);

    this.drawing.draw(gl, range);
};
Drink.prototype.set_position = function(pos)
{
    this.position = pos;
};
