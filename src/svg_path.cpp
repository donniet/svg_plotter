#include "svg_path.hpp"

#include <format>
#include <sstream>

using std::format;
using std::string;
using std::istream;
using std::stringstream;
using std::make_unique;
using std::logic_error;


void PathParser::parse(string const & s)
{
    stringstream ss(s);

    parse(ss);
}

istream & PathParser::parse(istream & is)
{
    do_parse(is);
    return is;
}

// eat any commas
void PathParser::eat_spaces_and_commas(istream & is)
{
    int commas_found = 0;
    while(is)
    {
        auto c = is.peek();
        if(c != ' ' && c != ',')
            break;

        if(c == ',')
        {
            if(commas_found > 0)
                throw ParseError(is.tellg(), "more than one comma found between numbers");

            commas_found++;
        }

        is.get();
    }
}

// eat spaces
void PathParser::eat_spaces(istream & is)
{
    while(is)
    {
        auto c = is.peek();
        if(c != ' ')
            break;

        is.get();
    }
}

double PathParser::expect_number(istream & is)
{
    auto pos = is.tellg();

    if(!is)
        throw ParseError(pos, "encountered EOF");

    double r;
    is >> r;

    if(is.fail())
        throw ParseError(pos, "error parsing number");

    eat_spaces_and_commas(is);

    return r;
}

PathParser::Part PathParser::part_from(char c)
{
    Part relativity = Absolute;

    switch(c) 
    {
    // Moves
    case 'm':
        relativity = Relative;
    case 'M':
        return (Part)(MoveTo | relativity);
    // Lines
    case 'l':
        relativity = Relative;
    case 'L':
        return (Part)(LineTo | relativity);

    case 'h':
        relativity = Relative;
    case 'H':
        return (Part)(Horizontal | relativity);
    
    case 'v':
        relativity = Relative;
    case 'V':
        return (Part)(Vertical | relativity);
    
    case 'a':
        relativity = Relative;
    case 'A':
        return (Part)(Arc | relativity);

    case 'c':
        relativity = Relative;
    case 'C':
        return (Part)(Bezier | Distinct | relativity);
    
    case 's':
        relativity = Relative;
    case 'S':
        return (Part)(Bezier | Continuation | relativity);
    
    case 'q':
        relativity = Relative;
    case 'Q':
        return (Part)(Quadratic | Distinct | relativity);
    
    case 't':
        relativity = Relative;
    case 'T':
        return (Part)(Quadratic | Continuation | relativity);

    case 'z':
        relativity = Relative;
    case 'Z':
        return (Part)(Close | relativity);

    case '-':
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return (Part)(Number);
    }

    return Unknown;
}

PathParser::Part PathParser::expect_command(istream & is)
{
    if(!is)
        return EndOfFile;

    // read in the next string (space delimited)
    string s;
    is >> s;

    if(is.eof() || s.size() == 0)
        return EndOfFile;

    Part p = part_from(s[0]);

    if(p & Command)
        return p;
    
    throw ParseError(is.tellg(), format("command not recognized: '{}'", s));
}

PathParser::Part PathParser::peek(istream & is)
{
    if(!is)
        throw ParseError(is.tellg(), "peek on a closed input stream");
    
    eat_spaces(is);

    return part_from(is.peek());
}


void PathParser::do_parse(istream & is)
{
    Part next;
    Point p{0,0};
    Vector v{0,0};
    double x, y, x1, y1, x2, y2, a, l, s, rx, ry;

    while(true)
    {
        next = expect_command(is);

        if(next == EndOfFile)
            break;

        switch(next & Command) {
        case MoveTo:
            do {
                p = pen();

                x = expect_number(is);
                y = expect_number(is);

                if(next & Absolute)
                    begin_path(x, y);
                else
                    begin_path(p.x + x, p.y + y);

            } while(peek(is) & Number);

            break;
        case LineTo:
            do {
                p = pen();

                x = expect_number(is);
                y = expect_number(is);

                if(next & Absolute)
                    line_to(x, y);
                else
                    line_to(p.x + x, p.y + y);
                
            } while(peek(is) & Number);

            break;   
        case Horizontal:
            do {
                p = pen();

                x = expect_number(is);

                if(next & Absolute)
                    line_to(x, p.y);
                else
                    line_to(p.x + x, p.y);

            } while(peek(is) & Number);

            break;
        case Vertical:
            do {
                p = pen();

                x = expect_number(is);

                if(next & Absolute)
                    line_to(p.x, y);
                else
                    line_to(p.x, p.y + y);
            } while(peek(is) & Number);

            break;
        case Arc:
            do {
                p = pen();

                rx = expect_number(is);
                ry = expect_number(is);
                a = expect_number(is);
                l = expect_number(is);
                s = expect_number(is);
                x = expect_number(is);
                y = expect_number(is);

                if(next & Absolute)
                    arc_to(rx, ry, a, l, s, x, y);
                else
                    arc_to(rx, ry, a, l, s, p.x + x, p.y + y);

            } while(peek(is) & Number);
            
            break;
        case Bezier:
            do {
                p = pen();
                v = direction(); 

                if(next & Continuation)
                {
                    // TODO: ensure that this handles absolute and relative properly
                    // if(is_absolute()) { ... }
                    x1 = v.x;
                    y1 = v.y;
                }
                else 
                {
                    x1 = expect_number(is);
                    y1 = expect_number(is);
                }
                x2 = expect_number(is);
                y2 = expect_number(is);
                x = expect_number(is);
                y = expect_number(is);

                if(next & Absolute)
                    bezier_to(x1, y1, x2, y2, x, y);
                else
                    bezier_to(p.x + x1, p.y + y1, p.x + x2, p.y + y2, p.x + x, p.y + y);

            } while(peek(is) & Number);

            break;
        
        case Quadratic:
            do {
                p = pen();

                if(next & Continuation)
                {
                    // TODO: ensure that this handles absolute and relative properly
                    // if(is_absolute()) { ... }
                    x1 = direction().x;
                    y1 = direction().y;
                }
                else 
                {
                    x1 = expect_number(is);
                    y1 = expect_number(is);
                }
                x = expect_number(is);
                y = expect_number(is);

                if(next & Absolute)
                    quadratic_to(x1, y1, x, y);
                else
                    quadratic_to(p.x + x1, p.y + y1, p.x + x, p.y + y);

            } while(peek(is) & Absolute);
            
            break;
        
        case Close:
            x = path_start_point().x;
            y = path_start_point().y;

            line_to(x, y);
            end_path();

            break;
        }
    }

    _visitor->end();
}

PathParser::Point PathParser::pen() 
{
    return _pen;
}

PathParser::Vector PathParser::direction()
{
    return _direction;
}

PathParser::Point PathParser::path_start_point()
{
    return _path_start;
}

void PathParser::line_to(double x, double y)
{
    _visitor->line(pen().x, pen().y, x, y);
}
void PathParser::arc_to(double rx, double ry, double angle, bool large_arc, bool sweep, double x, double y)
{
    _visitor->arc(pen().x, pen().y, rx, ry, angle, large_arc, sweep, x, y);
}
void PathParser::bezier_to(double x1, double y1, double x2, double y2, double x, double y)
{
    _direction = Vector{-x2, -y2};
    _visitor->bezier(pen().x, pen().y, x1, y1, x2, y2, x, y);
}
void PathParser::quadratic_to(double x1, double y1, double x, double y)
{
    _direction = Vector{-x1, -y1};
    _visitor->quadratic(pen().x, pen().y, x1, y1, x, y);
}
void PathParser::begin_path(double x, double y)
{
    _pen = Point{x,y};
    _direction = Vector{0,0};

    _visitor->begin(pen().x, pen().y);
}

void PathParser::end_path() 
{
    _direction = Vector{0,0};

    _visitor->end();
}

PathParser::PathParser() :
    _visitor(this), _current(Unknown), _path_start{0,0}, _pen{0,0}, _direction{0,0}
{ }

PathParser::PathParser(PathVisitor & visitor) : 
    PathParser()
{ 
    _visitor = &visitor;
}

PathParser::ParseError::ParseError(istream::pos_type pos, string const & msg) :
    logic_error(format("[{}] {}", (long)pos, msg))
{ }

