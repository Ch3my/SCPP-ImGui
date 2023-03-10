#include "../AppState.h"
#include "../src/helpers/ApiHelper.h"
#include "../src/helpers/ApiHelperC.h"
#include "../src/helpers/FormatNumber.h"
#include "../src/helpers/Utilities.h"
#include "../src/helpers/FileLogger.h"
#include "SingleDoc.h"
#include "../helpers/IconsMaterialDesign.h"
#include "TipoDocPicker.h"
#include "CategoriaPicker.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>
#include "../helpers/imguidatechooser.h"
#include <implot/implot.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <future>

#include <curl/curl.h>
#include <json/json.h>

namespace Documentos {
	// static para que no de error de linking por una variable del mismo nombre en otro archivo
	// Aunque los namespaces nos ayudan a evitar eso tambien
	static const char* dateFormat = "%d/%m/%Y";
	static std::future<Json::Value> promise;
	static std::future<void> promise_single_doc;
	static bool mounted = false;
	const float initial_window_width = 450.0f;
	const float initial_window_height = 680.0f;
	static bool show_debugger = false;
	static bool show_imgui_demo = false;
	static bool show_implot_demo = false;

	static Json::Value docs;
	static std::string sum_docs;

	static int fk_tipo_doc = 1; // 1 = Gasto, carga por defecto
	static int fk_categoria = 0; // 0 = Todos
	static tm fecha_inicio = { 0 };
	static tm fecha_termino = { 0 };
	static std::string searchPhrase = "";

	// Tenemos que declarar (dentro del namespace) para llamar antes de definir, o dar vuelta las funciones
	Json::Value get_documentos();
	void reload_docs();
	void preload_single_doc(int id_doc);

	void on_mounted() {
		// Seteamos fechas a las fechas que correspondan de inicio. Sino de Datechoose las setea
		// automaticamente a hoy al parecer
		Utilities::SetTmFromTipoDoc(fecha_inicio, fecha_termino, fk_tipo_doc);
		// Recargamos documentos
		reload_docs();
		mounted = true;
	}

	void reset() {
		fk_tipo_doc = 1;
		fk_categoria = 0;
		searchPhrase = "";
		Utilities::SetTmFromTipoDoc(fecha_inicio, fecha_termino, fk_tipo_doc);
		reload_docs();
	}

	void render() {
		if (!mounted) {
			on_mounted();
		}
		if (show_debugger) {
			ImGui::ShowMetricsWindow();
		}
		if (show_imgui_demo) {
			ImGui::ShowDemoWindow();
		}
		if (show_implot_demo) {
			ImPlot::ShowDemoWindow();
		}

		// Estas variables se pierden entre renderizaciones pero como son flags nomas
		// en teoria no hace diferencia, de usarse se usan durante el mismo ciclo que se definieron
		// asi podemos liberar memoria ?
		bool selected_row = false;
		bool tipo_doc_combo_changed = false;
		bool categoria_combo_changed = false;
		float CELL_PADDING_V = 7.0f;
		ImVec2 cell_padding(CELL_PADDING_V, CELL_PADDING_V);
		ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_PadOuterX;


		ImGui::SetNextWindowSize(ImVec2(initial_window_width, initial_window_height), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(
			ImVec2(
				ImGui::GetMainViewport()->Pos.x + 0,
				ImGui::GetMainViewport()->Pos.y + 0
			)
			, ImGuiCond_Appearing);

		// Aplica un stilo a la ventana
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
		// Es necesario setear la flag en la ventana para que muestre el menu
		// sino no lo muestra
		ImGui::Begin("Documentos", nullptr, ImGuiWindowFlags_MenuBar);

		// Esta pantalla se convirtio en la pantalla principal. Tiene menu para controlar otras opciones
		// como si se muestran o no los graficos
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Show Bar Graph", NULL, AppState::showBarGraph)) {
					// Si le hacen clic hacemos toggle de la variable que controla la ruta
					AppState::showBarGraph = !AppState::showBarGraph;
				}
				if (ImGui::MenuItem("Show Line Graph", NULL, AppState::showLineGraph)) {
					// Si le hacen clic hacemos toggle de la variable que controla la ruta
					AppState::showLineGraph = !AppState::showLineGraph;
				}
				if (ImGui::MenuItem("Show Debugger", NULL, show_debugger)) {
					show_debugger = !show_debugger;
				}
				if (ImGui::MenuItem("Show ImGui Demo", NULL, show_imgui_demo)) {
					show_imgui_demo = !show_imgui_demo;
				}
				if (ImGui::MenuItem("Show ImPlot Demo", NULL, show_implot_demo)) {
					show_implot_demo = !show_implot_demo;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// ____   _
		// | _ \ | |
		// ||_) || |_  _ __   ___
		// | _ < | __|| '_ \ / __|
		// | |_)|| |_ | | | |\__ \
		// |____/ \__||_| |_||___/
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_ADD_CIRCLE, ImVec2(30.0f, 30.0f))) {
			SingleDoc::reset(); // Resetamos antes por si acaso
			AppState::showSingleDoc = true;
		}
		if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))
			&& ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_A))
			&& !AppState::showSingleDoc) {
			SingleDoc::reset(); // Resetamos antes por si acaso
			AppState::showSingleDoc = true;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			reload_docs();
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_CLEAR_ALL, ImVec2(30.0f, 30.0f))) {
			reset();
		}
		ImGui::PopStyleVar();

		//	 ______  _  _  _
		//	| ____ |(_)| || |
		//	| |__    _ | || |_  ___   _ __  ___
		//	| __|   | || || __|/ _ \ | '__|/ __|
		//	| |     | || || |_ | __/ | |   \__ \
		//	|_|     |_||_| \__|\___| |_|  |___ /
		// Para que funcionen bonito tenemos que definirles su largo
		// porque tienen un largo por defecto que no nos acomoda
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
		TipoDocPicker::render(fk_tipo_doc, tipo_doc_combo_changed);
		if (fk_tipo_doc == 1) {
			// Solo mostramos categorias si el tipo es gasto
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			CategoriaPicker::render(fk_categoria, categoria_combo_changed);
		}
		ImGui::Text("Fecha Inicio");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DateChooser("##fecha_inicio", fecha_inicio, dateFormat, false, NULL, NULL, NULL, NULL, NULL)) {
			// Triggered on date change
			reload_docs();
		}
		ImGui::Text("Fecha Termino");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DateChooser("##fecha_termino", fecha_termino, dateFormat, false, NULL, NULL, NULL, NULL, NULL)) {
			// Triggered on date change
			reload_docs();
		}

		ImGui::Text("Buscar");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##searchPhraseinput", &searchPhrase);
		if (ImGui::IsItemDeactivated()) {
			// Triggered on "enter" or click outside
			reload_docs();
		}

		ImGui::Spacing();
		if (tipo_doc_combo_changed) {
			// Si cambian el tipo de documento nos aseguramos de cambiar el rango de fecha
			// segun corresponda
			Utilities::SetTmFromTipoDoc(fecha_inicio, fecha_termino, fk_tipo_doc);
			// Reseteamos la categoria. Ahorros y Ingresos no tienen y si la pasamos
			// va a filtrar por algo que no tiene sentido
			fk_categoria = 0;

			// Si el flag indica que el combo cambio (true aun si seleccionan el mismo)
			// llamamos a documentos en otro hilo, y otra pregunta revisa si el hilo devolvio
			// y carga los documentos
			reload_docs();
			// Reseteamos Flag para que no se ejecute de nuevo este codigo
			tipo_doc_combo_changed = false;
		}

		if (categoria_combo_changed) {
			reload_docs();
			categoria_combo_changed = false;
		}

		// Si la promesa esta ok usamos su resultado
		// _is_ready aun no esta ok. C++ esta "trabajando" en ello desde 2014 pero aun no esta listo para uso al parecer
		// sin .valid() se caia. Seria mas facil usar _is_ready nomas jajaj
		if (promise.valid() && promise.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			// al llamar .get ya promesa deja de _Is_ready() asi no se 
			// ejecuta mas de una vez este codigo
			docs = promise.get();
		}

		// Crea la tabla con flags, IMPORTANTE, los flags definen si se pueden reodenar y todas esas vainas
		// IMPORTANTE. La creacion de la tabla va en un IF porque sino al collapse la ventana da error
		// seguramente porque la tabla no existe y esta tratando de definir sobre algo que no existe, entonces
		// el if sino existe la tabla se salta todo ese codigo y no da error :D
		if (ImGui::BeginTable("docs", 3, table_flags)) {
			// Crea la tabla y configura las columnas, hay mas flags que se podrian aplicar
			ImGui::TableSetupColumn("Fecha", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Proposito");
			ImGui::TableSetupColumn("Monto", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableHeadersRow();

			// Primero hacemos loop en el array que no tiene nombre
			for (Json::Value::ArrayIndex i = 0; i != docs.size(); i++) {
				ImGui::TableNextRow();
				// TODO. Change row Color on Hover. Parece que no hay manera de hacer esto
				// habra que inventar algo o verlo en actualizacion de ImGui

				// Importante, los indices de columnas van desde 0, sino da error
				// seguramente por error de indice
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(docs[i]["fecha"].asString().c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Selectable(docs[i]["proposito"].asString().c_str(), &selected_row);
				if (selected_row) {
					// Seteamos documento en funcion que busca el documento
					// y lo carga
					preload_single_doc(docs[i]["id"].asInt());
					// ahora que ya deberia estar cargado marcamos para que se muestre la ventana
					AppState::showSingleDoc = true;
					selected_row = false;
				}

				ImGui::TableSetColumnIndex(2);

				// Aparentemente no existe una manera de alinear el texto a la derecha, asi que inventamos este
				// hack que calcula el ancho del texto y el espacio que sobra le pone un espacio en blanco
				float textWidth = ImGui::CalcTextSize(docs[i]["montoFormatted"].asString().c_str()).x;
				// Dummy crea un espacio en blanco con el alto y ancho que se especifica
				// GetColumnWidth obtiene el width de la columna en la tabla
				// GetColumnWidth no considera el Padding y queda parte del texto afuera
				// Debemos considerar en el calculo
				// Aparentemente ejecutar estos calculos aqui no afecta el rendimiento
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V * 2, 0.0f));
				ImGui::SameLine(); // Para que ambos elementos queden en la misma linea, sino no se ve el espacio
				ImGui::Text(docs[i]["montoFormatted"].asString().c_str());
			}
			ImGui::EndTable();

			// Si no hay documentos mostramos mensaje informativo
			if (docs.empty()) {
				const char* no_doc_msg = "No hay Documentos";
				// Para centrar el Texto. Se podria aplicar quizas tambien para alinear el los numeros a la derecha en la tabla
				// pero dejamos ambos a metodo de ejemplo
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(no_doc_msg).x) / 2.f);
				ImGui::Text(no_doc_msg);
			}

		}

		std::string total_text = "Total $ " + sum_docs;
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(total_text.c_str()).x - CELL_PADDING_V);
		ImGui::Text(total_text.c_str());

		ImGui::End();
		// Limpia un stilo a la ventana
		ImGui::PopStyleVar();
	}

	Json::Value get_documentos() {
		ApiHelperC apiHelperC;
		Json::Value json_args;
		json_args["sessionHash"] = AppState::sessionHash;
		json_args["fk_tipoDoc"] = fk_tipo_doc;
		json_args["fechaInicio"] = Utilities::FormatTm("%Y-%m-%d", fecha_inicio);
		json_args["fechaTermino"] = Utilities::FormatTm("%Y-%m-%d", fecha_termino);
		json_args["searchPhrase"] = searchPhrase;
		// 0 Significa todos. Ignoramos completamente si es cero
		if (fk_categoria != 0) {
			json_args["fk_categoria"] = fk_categoria;
		}
		int local_sum_docs = 0;

		//Json::Value data = ApiHelper::fn(AppState::apiPrefix + "/documentos", json_args, "GET");
		Json::Value data = apiHelperC.fn(AppState::apiPrefix + "/documentos", json_args, "GET");

		if (data == NULL) {
			FileLogger::log("Documentos.get_documentos() data == NULL. Error de comunicacion con la API?");
			return NULL;
		}
		// isMember crash si tratamos de llamarlo sobre un array
		// por eso verificamos que sea objeto primero
		if (data.isObject() && data.isMember("hasErrors")) {
			// Si hay errores no podemos iterar, debemos mostrar mensaje y salir
			FileLogger::log("Documentos.get_documentos() API responde hasErrors " + Utilities::json_to_string(data));
			return NULL;
		}

		// Formatear numeros aqui, hacerlo durante el runtime afecta los FPS reduciendo
		// un  50%, dicen que porque una funcion durante el render es demasiado lenta
		for (Json::Value::ArrayIndex i = 0; i != data.size(); i++) {
			std::string formattedNumber = FormatNumber::format(data[i]["monto"].asInt(), NULL, NULL);
			data[i]["montoFormatted"] = formattedNumber;
			local_sum_docs += data[i]["monto"].asInt();
		}
		sum_docs = FormatNumber::format(local_sum_docs, NULL, NULL);

		return data;
	}

	// No es static para que la llamen de otros archivos
	void reload_docs() {
		// Ejecuta Async, dentro de la funcion render se verifica si la promesa esta lista
		// y carga si corresponde, esa funcion se ejecuta constantemente, si quisieramos hacer algo asi aqui
		// tendriamos que hacer .get aqui mismo y entonces no seria multithread

		// Usamos std::async para llamar a la funcion, Disponible desde C++11
		// std::async con std::launch::async se asegura de ejecutar la funcion async, problablemente en otro thread
		// std::async se encarga de crear el thread o de usar uno que ya exista
		promise = std::async(std::launch::async, get_documentos);

		// NOTA. sino se esta ejecutando render la promesa queda por ahi dando vueltas??
		// porque no se llama a .get?
	}

	void preload_single_doc(int id_doc) {
		promise_single_doc = std::async(std::launch::async, SingleDoc::load_document, id_doc);
	}
}