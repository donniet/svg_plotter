#ifndef __SVG_PATH_HPP__
#define __SVG_PATH_HPP__


#include <string>
#include <istream>
#include <iterator>
#include <limits>
#include <array>
#include <memory>

class PathVisitor
{
public:
    virtual void begin(double, double) = 0;
    virtual void move(double, double, double, double) = 0; 
    virtual void line(double, double, double, double) = 0;
    virtual void arc(double, double, double, double, double, bool, bool, double, double) = 0;
    virtual void bezier(double, double, double, double, double, double, double, double) = 0;
    virtual void quadratic(double, double, double, double, double, double) = 0;
    virtual void end() = 0;
};

class PathParser : 
    public PathVisitor
{
private:
    typedef enum 
    {
        Unknown                 =  0x0000,

        Number                  =  0x0001,

        MoveTo                  =  0x0002,
        LineTo                  =  0x0004,
        Horizontal              =  0x0008,
        Vertical                =  0x000B,
        Arc                     =  0x0010,
        Bezier                  =  0x0020,
        Quadratic               =  0x0040,
        Close                   =  0x0080,
        Command                 =  0x00FF,

        Absolute                =  0x0100,
        Relative                =  0x0200,
        Relativity              =  0x0F00,

        Distinct                =  0x1000,
        Continuation            =  0x2000,
        Continuity              =  0xF000,

        EndOfFile               =  0xFFFF,
    } Part;


    struct ParseError : public std::logic_error
    { 
        ParseError(std::istream::pos_type pos, std::string const & msg);
    };

    static void eat_spaces_and_commas(std::istream &);
    static void eat_spaces(std::istream &);

    static Part part_from(char);
private:
    virtual void begin(double, double) override {};
    virtual void move(double, double, double, double) override {}; 
    virtual void line(double, double, double, double) override {};
    virtual void arc(double, double, double, double, double, bool, bool, double, double) override {};
    virtual void bezier(double, double, double, double, double, double, double, double) override {};
    virtual void quadratic(double, double, double, double, double, double) override {};
    virtual void end() override {};

private:
    struct Point { double x, y; };
    using Vector = Point;

    PathVisitor * _visitor;

    Part _current;
    
    Point _path_start;
    Point _pen;
    Vector _direction;

    Part expect_command(std::istream &);
    double expect_number(std::istream &);
    Part peek(std::istream &);

    Point pen();
    Vector direction();
    Point path_start_point();


    void begin_path(double, double);
    void line_to(double, double);
    void arc_to(double, double, double, bool, bool, double, double);
    void bezier_to(double, double, double, double, double, double);
    void quadratic_to(double, double, double, double);
    void end_path();
    

    void do_parse(std::istream &);

public:
    PathParser();
    PathParser(PathVisitor &);
    PathParser(PathParser const &) = default;
    PathParser(PathParser &&) = default;

    PathParser & operator=(PathParser const &) = default;
    PathParser & operator=(PathParser &&) = default;
    
    std::istream & parse(std::istream &);
    void parse(std::string const &);

};


#endif