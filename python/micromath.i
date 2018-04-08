%module micromath

%include <std_string.i>

%feature("python:slot", "tp_str", functype="reprfunc") vec3::string;

%{
//#include "micromath.h"
#include "vec3.h"
%}

//%include "micromath.h"
%include "vec3.h"

