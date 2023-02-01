#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../helpers/imguidatechooser.h"

#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/Utilities.h"
#include "./TipoDocPicker.h"
#include "./CategoriaPicker.h"
#include "../helpers/FormatNumber.h"

#include <json/json.h>

#include <iostream>
#include <string>


namespace SingleDoc {
	// Estos son los datos del documento actual
	// o del documento nuevo si estan creando uno
	static int id = NULL;
	static std::string proposito = "";
	static int monto = NULL;
	static std::string monto_text = "";
	// Inicializa como cero
	// tambien se podria setear a cero asi
	// strptime (timestamp_str, "%Y/%m/%d %H:%M:%S",&file_timestamp); ?
	static tm fecha = { 0 };
	const char* dateFormat = "%d/%m/%Y";
	int fk_tipo_doc = 1; // 1 = Gasto, carga por defecto
	int fk_categoria = 2; // 2 = Gasolina, carga por defecto

	void reset() {
		// Resetea valores de esta clase
		id = NULL;
		monto = NULL;
		monto_text = "";
		proposito = "";
		fecha = { 0 };
		fk_tipo_doc = 1;
		fk_categoria = 2;
	}

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
			// al guardar monto se convertida al int de monto_text, definimos aqui solo por estandar
			// para que no quede indefinida?
			monto = api_result[0]["monto"].asInt();
			// carga como numero/texto ya con separador de miles
			monto_text = FormatNumber::format(api_result[0]["monto"].asInt(), NULL, NULL);

			std::vector<std::string> t = Utilities::SplitString(api_result[0]["fecha"].asString(), "-");
			int year = std::stoi(t.at(0));
			int month = std::stoi(t.at(1));
			int day = std::stoi(t.at(2));
			Utilities::SetTmDate(fecha, year, month, day);

			// Carga tipoDoc y Categoria
			fk_tipo_doc = api_result[0]["fk_tipoDoc"].asInt();
			fk_categoria = api_result[0]["fk_categoria"].asInt();
		}
	}

	void render() {
		static bool show_window = true;
		if (!show_window) {
			reset(); // Limpiamos variables, no se mezclan variables si crean doc nuevo
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

		// Tratamos a monto como std::string porque tenemos que formatearlo con separador de miles
		// no acepta decimales ni otra cosa. ImGui parece que no tiene una manera de hacer esto
		// mejor
		ImGui::InputText("##montotext", &monto_text);
		// Equivalente al evento onChange de JS. Escucha por el elemento Anterior
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Si esta vacio no hacemos nada. Da error el ejecutar stoi en string vacio
			if (!monto_text.empty()) {
				monto_text = Utilities::get_digits(monto_text, false);
				monto_text = FormatNumber::format(std::stoi(monto_text), NULL, NULL);
			}
		}

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
		ImGui::SameLine(80);
		TipoDocPicker::render(fk_tipo_doc);

		if (fk_tipo_doc == 1) {
			// Solo mostramos categoria si tipo_doc es 1. Luego al guardar categoria pasara como null
			// si el tipo_doc es diferente de Gasto
			ImGui::Text("Categoria");
			ImGui::SameLine(80);
			CategoriaPicker::render(fk_categoria);
		}

		ImGui::End();
	}
}