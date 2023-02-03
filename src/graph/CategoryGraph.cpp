#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/Utilities.h"
#include "../helpers/IconsMaterialDesign.h"
#include "../helpers/TimerC.h"

#include <json/json.h>

#include <implot/implot.h>

#include <iostream>
#include <string>
#include <chrono>
#include <future>
#include <vector>

namespace CategoryGraph {

	static std::vector<int> bar_data;
	static std::vector<std::string> bar_labels;

	// Interface
	void before_render() {
	}

	// Interface
	void on_mounted() {

	}

	void get_data() {
		Json::Value args;
		args["sessionHash"] = AppState::sessionHash;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/expenses-by-category", args, "GET");

		// La api entrega varios objetos, recorremos el objeto consolidado y asignamos 
		// variables donde corresponda en un solo loop
		for (int i = 0; i < api_result["data"].size(); ++i) {
			const auto& object = api_result["data"][i];
			bar_data.push_back(object["data"].asInt());
			bar_labels.push_back(object["label"].asString());
		}
	}

	// Interface
	void render() {
		if (bar_data.empty()) {
			get_data();
		}
		// Los datos y las labels deberian tener siempre el mismo size()
		const int graph_size = bar_labels.size();

		// Obtiene acceso al array que el vector esta conteniendo
		// pasamos a imPLot y funciona!!
		int* bar_data_ptr = bar_data.data();

		ImGui::Begin("Gastos por Categoria Anual");

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			get_data();
		}
		ImGui::PopStyleVar();


		if (ImPlot::BeginPlot("Bar Plot")) {
			ImPlot::SetupAxis(ImAxis_X1, "Categoria");
			ImPlot::SetupAxis(ImAxis_Y1, "Monto");

			// I created B on the heap using new because in the original prompt the requirement was 
			// to generate an array with the size of the std::vector, which is 
			// determined at runtime.When you create an array using new, the size can be 
			// determined dynamically at runtime.On the other hand, if you create an array 
			// using double B[n]; syntax, the size n must be a constant expression known at compile - time, 
			// which may not always be the case.
			double* positions = new double[graph_size];
			for (int i = 0; i < graph_size; ++i) {
				positions[i] = i;
			}

			// Note that because the elements of char_pointer_vector store pointers to the 
			// underlying data of string_vector, it's important that the 
			// elements of string_vector remain valid for the lifetime of the char* array, labels.
			std::vector<const char*> char_pointer_vector;
			char_pointer_vector.reserve(graph_size);
			for (const auto& s : bar_labels) {
				char_pointer_vector.push_back(s.c_str());
			}
			const char** labels = char_pointer_vector.data();

			ImPlot::SetupAxisTicks(ImAxis_X1, positions, graph_size, labels);

			ImPlot::PlotBars("##graph1", bar_data_ptr, graph_size, 0.5, 0);

			ImPlot::EndPlot();

			// No debemos olvidar delete el Arr porque lo creamos en el Heap
			// para poder definir su tamaño en runtime
			delete[] positions;
		}

		ImGui::End();
	}
}