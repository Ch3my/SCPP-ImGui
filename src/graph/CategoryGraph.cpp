#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>

#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/Utilities.h"
#include "../helpers/IconsMaterialDesign.h"
#include "../helpers/TimerC.h"
#include "../src/helpers/FormatNumber.h"

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
	static bool show_window = true;
	static ImVec4 color = ImVec4(0.323f, 0.819f, 0.319f, 1.0f);

	// TODO. Interface
	void before_render() {
	}

	// TODO.Interface
	void on_mounted() {

	}

	void get_data() {
		Json::Value args;
		args["sessionHash"] = AppState::sessionHash;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/expenses-by-category", args, "GET");

		// La api entrega varios objetos, recorremos el objeto consolidado y asignamos 
		// variables donde corresponda en un solo loop
		// unsigned int i  porque .size devuelve unsign. VS daba mensaje de advertencia
		for (unsigned int i = 0; i < api_result["data"].size(); ++i) {
			const auto& object = api_result["data"][i];
			bar_data.push_back(object["data"].asInt());
			if (object["label"].asString().size() > 6) {
				// Limita el largo de la palabra
				// lo ideal es rotar las labels pero parece que no se puede
				bar_labels.push_back(object["label"].asString().substr(0, 6));
				continue;
			}
			bar_labels.push_back(object["label"].asString());
		}
	}

	int YAxisFormatter(double value, char* buff, int size, void* data) {
		// Sacado de implot_Demo funcion MetricFormatter. Usar FormatNumber no afecta la
		// performance asi que usamos nomas
		return snprintf(buff, size, "%s", FormatNumber::format(int(value), NULL, NULL).c_str());
	}

	// TODO. Interface
	void render() {
		if (bar_data.empty()) {
			get_data();
		}
		// Los datos y las labels deberian tener siempre el mismo size()
		// uso size_t porque habia mensaje en VS funciona size_t/int sin diferencia
		const auto graph_size = bar_labels.size();

		// Obtiene acceso al array que el vector esta conteniendo
		// pasamos a imPLot y funciona!!
		int* bar_data_ptr = bar_data.data();

		ImGui::SetNextWindowSize(ImVec2(650.0f, 400.0f));
		// show_window solo esta para llenar el espacio, no se puede cerrar esta ventana
		// TODO ver si sera necesario cerrar esta ventana. Si se cierra hay que crear manera de abrirla de nuevo
		ImGui::Begin("Gastos por Categoria Anual", &show_window, ImGuiWindowFlags_NoResize);

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			get_data();
		}
		ImGui::PopStyleVar();

		// Buscamos las fuentes cargadas y asignamos una mas pequeña al grafico
		// para que se vean los datos
		ImGuiContext* GImGui = ImGui::GetCurrentContext();
		ImGuiContext& g = *GImGui;
		ImFontAtlas* atlas = g.IO.Fonts;
		ImGui::PushFont(atlas->Fonts[1]);

		// ImPlotFlags_NoMouseText no muestra coordenadas del Mouse cuando pasea

		if (ImPlot::BeginPlot("Gastos por Categoria Anual", ImVec2(-1, -1), ImPlotFlags_NoMouseText)) {
			ImPlot::SetupAxis(ImAxis_X1, "Categoria");
			ImPlot::SetupAxis(ImAxis_Y1, "Monto");
			// Pone el limite de Y Axis 10% mayor que el valor mas alto del dataset
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, *std::max_element(bar_data.begin(), bar_data.end()) * 1.1);

			// Define and use a custom formatter, formatea datos de Y
			ImPlot::SetupAxisFormat(ImAxis_Y1, YAxisFormatter, (void*)"");

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
			ImPlot::PushStyleColor(ImPlotCol_Fill, color);
			ImPlot::PlotBars("Mnt Cat", bar_data_ptr, graph_size, 0.5, 0);
			// custom legend context menu
			if (ImPlot::BeginLegendPopup("Mnt Cat")) {
				ImGui::ColorEdit3("Color", &color.x);
				ImPlot::EndLegendPopup();
			}
			ImPlot::PopStyleColor();
			ImPlot::EndPlot();

			// No debemos olvidar delete el Arr porque lo creamos en el Heap
			// para poder definir su tamaño en runtime
			delete[] positions;
		}
		ImGui::PopFont();

		ImGui::End();
	}
}