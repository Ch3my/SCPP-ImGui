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
#include <future>
#include <thread>

namespace TimeGraph {
	static bool show_window = true;
	static bool mounted = false;
	static std::vector<double> x_axis_timestamps;
	static std::vector<double> gastos_line;
	static std::vector<double> ahorros_line;
	static std::vector<double> ingresos_line;
	static const int meses_consulta = 7;
	static std::future<void> promise;
	// Atomic en realidad no es necesario
	static std::atomic<bool> refreshing_data = false;

	float CELL_PADDING_V = 3.0f;
	ImVec2 cell_padding(CELL_PADDING_V, CELL_PADDING_V);
	ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_PadOuterX;

	// Tenemos que pasar double[], usando vector.data() no funciona, asi que lo hacemos
	// a mano. Por seguridad este lo creamos en el Stack
	// ==== Variables para ImPlot ====
	double gastos_arr[meses_consulta];
	double ingresos_arr[meses_consulta];
	double ahorros_arr[meses_consulta];

	double max_value_datasets = 0;
	static ImVec4 gastos_color = ImVec4(0.776f, 0.0f, 0.157f, 1.0f);
	static ImVec4 ahorros_color = ImVec4(0.895f, 0.728f, 0.015f, 1.0f);
	static ImVec4 ingresos_color = ImVec4(0.354f, 0.420f, 1.0f, 1.0f);

	// Cargamos variables como JSON para centralizarlas y recorrer mas facil
	static Json::Value table_data;

	void get_data() {
		Json::Value args;
		args["sessionHash"] = AppState::sessionHash;
		args["nMonths"] = meses_consulta;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/monthly-graph", args, "GET");

		refreshing_data = true;

		// Resetea variables. Evita que crezca la tabla con datos repetidos
		gastos_line.clear();
		ingresos_line.clear();
		ahorros_line.clear();
		x_axis_timestamps.clear();
		table_data.clear();

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

			// Guardamos datos para que la tabla pueda leer
			Json::Value this_row;
			this_row["label"] = labels[i].asString();
			this_row["gasto"] = FormatNumber::format(gastos[i].asInt(), NULL, NULL);
			this_row["ahorro"] = FormatNumber::format(ahorros[i].asInt(), NULL, NULL);
			this_row["ingreso"] = FormatNumber::format(ingresos[i].asInt(), NULL, NULL);
			table_data.append(this_row);
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

	void render() {
		if (!show_window) {
			AppState::showLineGraph = false;
			mounted = false;
			show_window = true;
		}
		if (x_axis_timestamps.size() == 0 && !mounted) {
			reload_data();
		}

		ImGui::SetNextWindowSize(ImVec2(750.0f, 320.0f));
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
		if (!refreshing_data && 
			!x_axis_timestamps.empty() &&
			ImPlot::BeginPlot("Gastos por Categoria Anual",
			ImVec2(ImGui::GetContentRegionAvail().x * 0.55f, -1),
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

			ImPlot::SetNextLineStyle(ahorros_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Ahorros", timestamps_ptr, ahorros_arr, meses_consulta);
			ImPlot::SetNextLineStyle(ingresos_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Ingresos", timestamps_ptr, ingresos_arr, meses_consulta);
			// Dejamos gastos al final para que la linea quede sobre las demas
			ImPlot::SetNextLineStyle(gastos_color, 2);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("Gastos", timestamps_ptr, gastos_arr, meses_consulta);

			ImPlot::EndPlot();
		}
		ImGui::SameLine();
		if (!refreshing_data && ImGui::BeginTable("line_graph", 4, table_flags)) {
			// Crea la tabla y configura las columnas, hay mas flags que se podrian aplicar
			ImGui::TableSetupColumn("Mes");
			ImGui::TableSetupColumn("Ingresos");
			ImGui::TableSetupColumn("Gastos");
			ImGui::TableSetupColumn("Ahorros");
			ImGui::TableHeadersRow();
			for (Json::Value::ArrayIndex i = 0; i != table_data.size(); i++) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(table_data[i]["label"].asString().c_str());
				ImGui::TableSetColumnIndex(1);
				// Hack Alinear texto a la derecha
				float textWidth = ImGui::CalcTextSize(table_data[i]["ingreso"].asString().c_str()).x;
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V * 2, 0.0f));
				ImGui::SameLine();
				ImGui::Text(table_data[i]["ingreso"].asString().c_str());
				ImGui::TableSetColumnIndex(2);
				textWidth = ImGui::CalcTextSize(table_data[i]["gasto"].asString().c_str()).x;
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V * 2, 0.0f));
				ImGui::SameLine();
				ImGui::Text(table_data[i]["gasto"].asString().c_str());
				ImGui::TableSetColumnIndex(3);
				textWidth = ImGui::CalcTextSize(table_data[i]["ahorro"].asString().c_str()).x;
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V * 2, 0.0f));
				ImGui::SameLine();
				ImGui::Text(table_data[i]["ahorro"].asString().c_str());
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