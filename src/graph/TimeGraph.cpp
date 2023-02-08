#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>

#include "../src/helpers/FormatNumber.h"
#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/IconsMaterialDesign.h"

#include <json/json.h>

#include <implot/implot.h>

#include <iostream>
#include <string>

namespace TimeGraph {
	static bool show_window = true;
	static bool mounted = false;
	static std::vector<double> x_axis_timestamps;
	static std::vector<double> gastos_line;
	static std::vector<double> ahorros_line;
	static std::vector<double> ingresos_line;
	static const int meses_consulta = 7;

	// Tenemos que pasar double[], usando vector.data() no funciona, asi que lo hacemos
	// a mano. Por seguridad este lo creamos en el Stack
	double gastos_arr[meses_consulta];
	double ingresos_arr[meses_consulta];
	double ahorros_arr[meses_consulta];

	double max_value_datasets = 0;

	static ImVec4 gastos_color = ImVec4(0.776f, 0.0f, 0.157f, 1.0f);
	static ImVec4 ahorros_color = ImVec4(0.895f, 0.728f, 0.015f, 1.0f);
	static ImVec4 ingresos_color = ImVec4(0.354f, 0.420f, 1.0f, 1.0f);

	float CELL_PADDING_V = 3.0f;
	ImVec2 cell_padding(CELL_PADDING_V, CELL_PADDING_V);
	ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX;

	void get_data() {
		Json::Value args;
		args["sessionHash"] = AppState::sessionHash;
		args["nMonths"] = meses_consulta;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/monthly-graph", args, "GET");

		// Todos los objetos deben traer el mismo size. consultamos solo para loop
		for (unsigned int i = 0; i < api_result["labels"].size(); ++i) {
			const auto& labels = api_result["labels"];
			const auto& gastos = api_result["gastosDataset"];
			const auto& ingresos = api_result["ingresosDataset"];
			const auto& ahorros = api_result["ahorrosDataset"];

			gastos_line.push_back(gastos[i].asDouble());
			ingresos_line.push_back(ingresos[i].asDouble());
			ahorros_line.push_back(ahorros[i].asDouble());

			// Tenemos que guardar el valor maximo de cualquiera de los dataset
			// para pasarselo al grafico y se vea bonito
			if (max_value_datasets < gastos[i].asDouble()) {
				max_value_datasets = gastos[i].asDouble();
			}
			if (max_value_datasets < ingresos[i].asDouble()) {
				max_value_datasets = ingresos[i].asDouble();
			}
			if (max_value_datasets < ahorros[i].asDouble()) {
				max_value_datasets = ahorros[i].asDouble();
			}

			// Convertimos string YYYY-MM a epoch para pasar a
			// implot y que lo pueda interpretar como fechas automaticamente
			std::tm t{};
			std::istringstream ss(labels[i].asString());
			ss >> std::get_time(&t, "%Y-%m");
			if (ss.fail()) {
				throw std::runtime_error{ "failed to parse time string" };
			}
			std::time_t time_stamp = mktime(&t);
			// Pasamos a Doble porque eso utiliza ImPLot. Luego de todas maneras tenemos que convertir
			// a double[]
			x_axis_timestamps.push_back(static_cast<double>(time_stamp));
		}

		// Preparamos datos para el grafico aqui. Asi no lo hacemos durante la renderizacion
		int g_counter = 0;
		for (double g : gastos_line) {
			gastos_arr[g_counter] = g;
			g_counter++;
		}
		int a_counter = 0;
		for (double a : ahorros_line) {
			ahorros_arr[a_counter] = a;
			a_counter++;
		}
		int i_counter = 0;
		for (double i : ingresos_line) {
			ingresos_arr[i_counter] = i;
			i_counter++;
		}

	}
	int YAxisFormatter(double value, char* buff, int size, void* data) {
		// Sacado de implot_Demo funcion MetricFormatter. Usar FormatNumber no afecta la
		// performance asi que usamos nomas
		return snprintf(buff, size, "%s", FormatNumber::format(int(value), NULL, NULL).c_str());
	}

	void render() {
		if (!show_window) {
			AppState::showLineGraph = false;
			mounted = false;
			show_window = true;
		}
		if (x_axis_timestamps.size() == 0) {
			get_data();
		}

		ImGui::SetNextWindowSize(ImVec2(700.0f, 400.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);

		ImGui::Begin("Historico por Tipo Doc", &show_window, ImGuiWindowFlags_NoResize);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			get_data();
		}
		ImGui::PopStyleVar();

		// ImPlotFlags_NoMouseText no muestra coordenadas del Mouse cuando pasea		
		// ImPlotFlags_NoTitle no muestra el titulo porque la ventana ya tiene titulo
		// Definimos el largo del plot para poder tener espacio para tabla al costado
		if (ImPlot::BeginPlot("Gastos por Categoria Anual",
			ImVec2(ImGui::GetContentRegionAvail().x / 1.25f, -1),
			ImPlotFlags_NoMouseText | ImPlotFlags_NoTitle)) {
			ImPlot::SetupAxis(ImAxis_Y1, "Monto");
			ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
			// Define and use a custom formatter, formatea datos de Y
			ImPlot::SetupAxisFormat(ImAxis_Y1, YAxisFormatter, (void*)"");
			// Posicion del cuadro descriptivo o legenda
			ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal);

			// Pone el limite de Y Axis 10% mayor que el valor mas alto del dataset
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, max_value_datasets * 1.1);

			double* timestamps_ptr = x_axis_timestamps.data();

			ImPlot::SetNextLineStyle(gastos_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Gastos", timestamps_ptr, gastos_arr, meses_consulta);
			ImPlot::SetNextLineStyle(ahorros_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Ahorros", timestamps_ptr, ahorros_arr, meses_consulta);
			ImPlot::SetNextLineStyle(ingresos_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Ingresos", timestamps_ptr, ingresos_arr, meses_consulta);

			ImPlot::EndPlot();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}