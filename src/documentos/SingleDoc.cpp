#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../helpers/imguidatechooser.h"

#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/Utilities.h"

#include <json/json.h>

#include <iostream>
#include <string>


namespace SingleDoc {
	// Estos son los datos del documento actual
	// o del documento nuevo si estan creando uno
	static int id = NULL;
	static std::string proposito = "";
	static int monto = NULL;
	// Inicializa como cero
	// tambien se podria setear a cero asi
	// strptime (timestamp_str, "%Y/%m/%d %H:%M:%S",&file_timestamp); ?
	static tm fecha = { 0 };
	const char* dateFormat = "%d/%m/%Y";

	void load_document(int id) {
		// Nos pasan un id de Documento y cargamos sus datos en la clase
		Json::Value args;
		args["id"][0] = id;
		args["sessionHash"] = AppState::sessionHash;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/documentos", args, "GET");
		std::cout << api_result;
		// Deberia venir solo 1 resultado
		if (api_result.isArray()) {
			proposito = api_result[0]["proposito"].asString();
			monto = api_result[0]["monto"].asInt();

			std::vector<std::string> t = Utilities::SplitString(api_result[0]["fecha"].asString(), "-");

			memset(&fecha, 0, sizeof(tm));     // Mandatory for emscripten. Please do not remove!
			fecha.tm_isdst = -1;
			fecha.tm_sec = 0;		//	 Seconds.	[0-60] (1 leap second)
			fecha.tm_min = 0;		//	 Minutes.	[0-59]
			fecha.tm_hour = 0;		//	 Hours.	[0-23]
			fecha.tm_wday = 0;     //	 Day of week.	[0-6]
			fecha.tm_yday = 0;		//	 Days in year.[0-365]

			// El año se cuenta relativo a 1900. Entonces si estamos en 2023 el año es 123 (2023 - 1900)
			fecha.tm_year = std::stoi(t.at(0)) - 1900;
			// El mes comienza el mes 0
			fecha.tm_mon = std::stoi(t.at(1)) - 1;
			fecha.tm_mday = std::stoi(t.at(2));
		}
	}

	void render() {
		static bool show_window = true;
		if (!show_window) {
			AppState::showSingleDoc = false;
			show_window = true;
		}

		// Si estamos en esta funcion y fecha no esta definido lo definimos
		// eso ocurriria en documentos nuevos, ya que editando documentos la fecha
		// ya debio ser llenada en load_document
		if (fecha.tm_mday == 0) {
			ImGui::SetDateToday(&fecha);
		}

		// Setea el tamaño de la ventana
		ImGui::SetNextWindowSize(ImVec2(350.0f, 300.0f));
		ImGui::Begin("Single Doc", &show_window, ImGuiWindowFlags_NoResize);

		ImGui::Text("Monto");
		// Seteamos un espacion entre items, El mismo espacio para mantener
		// la linea con los otros inputs del formulario
		ImGui::SameLine(80);
		ImGui::InputInt("##inputmonto", &monto);

		ImGui::Text("Proposito");
		ImGui::SameLine(80);
		ImGui::InputText("##propositoinput", &proposito);

		ImGui::Text("Fecha");
		ImGui::SameLine(80);
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.97f);
		if (ImGui::DateChooser("##DateChooserid", fecha, dateFormat, false, NULL, NULL, NULL, NULL, NULL)) {
			// Triggered on date change
		}
		ImGui::PopItemWidth();


		ImGui::Text("Tipo Doc");
		ImGui::Text("Categoria");

		ImGui::End();
	}
}