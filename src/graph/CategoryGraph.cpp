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
#include <mutex>
#include <atomic>

namespace CategoryGraph {

	static std::vector<int> bar_data;
	static std::vector<std::string> formatted_bar_data;
	static std::vector<std::string> bar_labels;
	static bool show_window = true;
	static bool mounted = false;
	static std::future<void> promise;
	// Atomic en realidad no es necesario
	static std::atomic<bool> refreshing_data = false;
	const float initial_window_width = 750.0f;

	static ImVec4 color = ImVec4(0.323f, 0.819f, 0.319f, 1.0f);
	float CELL_PADDING_V = 3.0f;
	ImVec2 cell_padding(CELL_PADDING_V, CELL_PADDING_V);
	ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX;

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

		refreshing_data = true;
		// Reseteamos variables
		bar_data.clear();
		bar_labels.clear();
		formatted_bar_data.clear();

		// La api entrega varios objetos, recorremos el objeto consolidado y asignamos 
		// variables donde corresponda en un solo loop
		// unsigned int i  porque .size devuelve unsign. VS daba mensaje de advertencia
		for (unsigned int i = 0; i < api_result["data"].size(); ++i) {
			const auto& object = api_result["data"][i];
			bar_data.push_back(object["data"].asInt());
			formatted_bar_data.push_back(FormatNumber::format(object["data"].asInt(), NULL, NULL));

			// LABELS
			if (object["label"].asString().size() > 5) {
				// Limita el largo de la palabra
				// lo ideal es rotar las labels pero parece que no se puede
				bar_labels.push_back(object["label"].asString().substr(0, 5));
				continue;
			}
			bar_labels.push_back(object["label"].asString());
		}
		refreshing_data = false;
	}

	int YAxisFormatter(double value, char* buff, int size, void* data) {
		// Sacado de implot_Demo funcion MetricFormatter. Usar FormatNumber no afecta la
		// performance asi que usamos nomas
		return snprintf(buff, size, "%s", FormatNumber::format(int(value), NULL, NULL).c_str());
	}

	// No es static para que la llamen de otros archivos
	void reload_data() {
		// Ejecuta Async, dentro de la funcion render se verifica si la promesa esta lista
		// y carga si corresponde, esa funcion se ejecuta constantemente, si quisieramos hacer algo asi aqui
		// tendriamos que hacer .get aqui mismo y entonces no seria multithread

		// Usamos std::async para llamar a la funcion, Disponible desde C++11
		// std::async con std::launch::async se asegura de ejecutar la funcion async, problablemente en otro thread
		// std::async se encarga de crear el thread o de usar uno que ya exista
		promise = std::async(std::launch::async, get_data);

		// NOTA. sino se esta ejecutando render la promesa queda por ahi dando vueltas??
		// porque no se llama a .get?
	}

	// TODO. Interface
	void render() {
		if (!show_window) {
			AppState::showBarGraph = false;
			mounted = false;
			show_window = true;
		}

		// Llenamos datos si la lista esta vacia y aun no estamos mounted
		// sino estariamos llamando continuamente si por alguna razon
		// la lista viene vacia desde la API
		if (bar_data.empty() && !mounted) {
			reload_data();
		}
		// Los datos y las labels deberian tener siempre el mismo size()
		// uso size_t porque habia mensaje en VS funciona size_t/int sin diferencia
		const auto graph_size = bar_labels.size();

		// Obtiene acceso al array que el vector esta conteniendo
		// pasamos a imPLot y funciona!!
		int* bar_data_ptr = bar_data.data();

		// Seteamos el size de la window solo la primera vez
		ImGui::SetNextWindowSize(ImVec2(initial_window_width, 360.0f), ImGuiCond_Appearing);

		// Seteamos la ubicacion inicialmente. El usuario puede mover donde quiera despues
		// La primera posicion se considera relativa al mainViewport. La ventana no es parte del
		// mainViewport la podemos sacar luego si queremos o volver a poner
		ImGui::SetNextWindowPos(
			ImVec2(
				ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x - initial_window_width,
				ImGui::GetMainViewport()->Pos.y + 0
			)
		,ImGuiCond_Appearing);

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
		ImGui::Begin("Gastos por Categoria Anual", &show_window);

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			reload_data();
		}
		ImGui::PopStyleVar();

		// Buscamos las fuentes cargadas y asignamos una mas pequeña al grafico
		// para que se vean los datos
		ImGuiContext* GImGui = ImGui::GetCurrentContext();
		ImGuiContext& g = *GImGui;
		ImFontAtlas* atlas = g.IO.Fonts;
		ImGui::PushFont(atlas->Fonts[1]);

		// ImPlotFlags_NoMouseText no muestra coordenadas del Mouse cuando pasea		
		// ImPlotFlags_NoTitle no muestra el titulo porque la ventana ya tiene titulo
		// Definimos el largo del plot para poder tener espacio para tabla al costado

		// Solo renderizamos si hay datos, sino nos salimos. Verificamos si hay datos primero
		// si verificamos segundo el plot se alcanza a crear
		if (!bar_data.empty() && !refreshing_data && 
			ImPlot::BeginPlot("Gastos por Categoria Anual",
			ImVec2(ImGui::GetContentRegionAvail().x / 1.25f, -1),
			ImPlotFlags_NoMouseText | ImPlotFlags_NoTitle)) {

			ImPlot::SetupAxis(ImAxis_X1, "Categoria");
			ImPlot::SetupAxis(ImAxis_Y1, "Monto");
			// Posicion del cuadro descriptivo o legenda
			ImPlot::SetupLegend(ImPlotLocation_NorthEast);
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
		ImGui::SameLine();

		// ==== Tabla de Datos ====
		if (!refreshing_data && ImGui::BeginTable("docs", 2, table_flags)) {
			// Crea la tabla y configura las columnas, hay mas flags que se podrian aplicar
			ImGui::TableSetupColumn("Cat");
			ImGui::TableSetupColumn("Monto");
			ImGui::TableHeadersRow();
			for (Json::Value::ArrayIndex i = 0; i != formatted_bar_data.size(); i++) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(bar_labels[i].c_str());
				ImGui::TableSetColumnIndex(1);
				// Hack Alinear texto a la derecha
				float textWidth = ImGui::CalcTextSize(formatted_bar_data[i].c_str()).x;
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V*2, 0.0f));
				ImGui::SameLine();
				ImGui::Text(formatted_bar_data[i].c_str());
			}
			ImGui::EndTable();
		}

		ImGui::End();
		ImGui::PopStyleVar();

		// AL final de la primera renderizacion seteamos mounted a true para avisar que se hicieron
		// todas las operaciones correspondientes y/o llamar otras operaciones
		if (!mounted) {
			mounted = true;
		}
	}
}