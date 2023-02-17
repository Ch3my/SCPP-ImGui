#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

#include "Utilities.h"

namespace FileLogger {
	static std::ofstream logfile;
	static std::mutex lock;

	void create_log_file() {
		logfile.open("log.txt", std::ios::out | std::ios::trunc);
		logfile.close();
	}

	void log(const std::string& msg) {
		// Hacemos lock, evitamos errores al log desde diferentes threads
		std::lock_guard<std::mutex> lock_guard(lock);
		
		std::string full_msg = "";
		// Si el archivo no esta abierto lo abrimos
		if (!logfile) {
			// Se abre modo Append para no borrar Logs Antiguos
			logfile.open("log.txt", std::ios::app);
			if (!logfile) {
				std::cout << "No se pudo abrir log.txt" << std::endl;
				std::cout << "Creamos archivo log.txt" << std::endl;
				create_log_file();
				return;
			}
		}

		full_msg += Utilities::get_current_time_and_date() + " > ";
		full_msg += msg + "\n";
		logfile << full_msg;
		// Deja archivo abierto para futuros Logs
		// ofstream cierra el archivo en el destructor de todas maneras
	}
}