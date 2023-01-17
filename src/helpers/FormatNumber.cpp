#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>

namespace FormatNumber {
	// TODO crear Overloads para Double, float

	std::string format(int number, int outputWidth, char widthFill) {
		// TODO. configurar para diferentes formatos de numeros
		// Formatea el Numero a la 
		// para establecer cuantos decimales
		// std::cout.precision(3);
		// para establecer largo
		// std::setw(10)
		// para establecer fill para llegar al largo
		// std::setfill('0')
		// para establecer el Locale
		// std::cout.imbue(std::locale("")); 

		// Este stream se usara para hacer el formato del numero
		// setw, locale y todos esos metodos aplicaran sobre ss
		// para configurar el stream como queremos
		std::stringstream ss;
		// Para setear la locale por defecto del SO
		ss.imbue(std::locale(""));
		if (outputWidth != NULL) {
			ss.width(outputWidth);
		}
		if (widthFill != NULL) {
			ss.fill(widthFill);
		}
		ss << number;

		return ss.str();
	}
}