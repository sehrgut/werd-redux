#ifndef WERD_EVALUATE_CLASSDEF_H
#define WERD_EVALUATE_CLASSDEF_H

char* evaluate_class(Classdef* c, Classdef** cc, int n);
char* evaluate_class_by_name(char* name, Classdef** cc, int n);

Classdef* find_class_by_name(char* name, Classdef** cc, int n);

#endif
