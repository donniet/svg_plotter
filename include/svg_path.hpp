#ifndef __SVG_PATH_HPP__
#define __SVG_PATH_HPP__



class PathParser 
{
private:
    typedef enum 
    {
        Number,
        MoveTo,
        LineTo,
        HorizontalLine,
        VerticalLine,
        Arc,
        Bezier,
        BezierContinuation,
        Quadratic,
        QuadraticContinuation,
        Close
    } Part;

    struct Token 
    {
        Part part;
        double value;
    };


public:
    Token next();
    

};

#endif