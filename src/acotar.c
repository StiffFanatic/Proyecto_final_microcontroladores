
#include "acotar.h"
#define BORDE_INTEGRAL 120

/// Para evitar retardos en respuesta

double acotar_integral(double integral){

    if(integral > BORDE_INTEGRAL){
        integral = BORDE_INTEGRAL;
    }
    else if(integral < -(BORDE_INTEGRAL)){
        integral = -(BORDE_INTEGRAL);
    }

    return integral;
}

double acotar_salida(double salida, int desde, int hasta){

    if(salida > hasta){
        salida = hasta;
    }
    else if(salida < desde){
        salida = desde;
    }

    return salida;
}