#pragma once
// Este es un archivo que pretende generar la funcion setTimeout de JS en C++
// Seguramente hay una manera usando templates o algo asi para poder tener varios tipos de 
// return con la misma funcion pero por ahora solo implementaremos la funcion segun el tipo

// basado en https://stackoverflow.com/questions/50691492/setting-timeout-for-c-c-function-call
// y https://www.geeksforgeeks.org/passing-a-function-as-a-parameter-in-cpp/

// Autor: Chemy

#include <future>
#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <thread>

namespace TimerC {
	// Funcion retorna Int y ejecuta funcion sin Argumentos
	std::future<int> fn(int delay, std::function<int()> func) {
		// delay en ms
		// funcion pasada como &func : sin ()

		/*std::future<int> future = std::async(std::launch::async, func);
		std::future_status status = future.wait_for(std::chrono::milliseconds(delay));
		// NOTA. como future.get() bloquea quiza podamos bloquear hasta que termine la funcion sino ha terminado ??
		if (status == std::future_status::timeout) {
			// func is not complete.
			return 1;
		}
		else if (status == std::future_status::ready) {
			// verySlow() is complete.
			// Get result from future (if there's a need)
			auto ret = future.get();
			return ret;
		} */

		std::future<int> future = std::async(std::launch::async, [func, delay]() {
			std::chrono::milliseconds dura(delay);
			std::this_thread::sleep_for(dura);
			int result = func();
			return result;
			});

		//auto ret = future.get();
		//return ret;
		return future;
	}
}

