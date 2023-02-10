#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../helpers/imguidatechooser.h"

#include "../AppState.h"
#include "../helpers/ApiHelper.h"
#include "../helpers/Utilities.h"
#include "./TipoDocPicker.h"
#include "./CategoriaPicker.h"
#include "../helpers/FormatNumber.h"
#include "../helpers/IconsMaterialDesign.h"
#include "../helpers/TimerC.h"
#include "documentos.h"

#include <json/json.h>

#include <iostream>
#include <string>
#include <chrono>
#include <future>

namespace SingleDoc {
	// Estos son los datos del documento actual
	// o del documento nuevo si estan creando uno
	static int id = NULL;
	static std::string proposito = "";
	static std::string monto_text = "";
	// Inicializa como cero
	// tambien se podria setear a cero asi
	// strptime (timestamp_str, "%Y/%m/%d %H:%M:%S",&file_timestamp); ?
	static tm fecha = { 0 };
	static int fk_tipo_doc = 1; // 1 = Gasto, carga por defecto
	static int fk_categoria = 2; // 2 = Gasolina, carga por defecto
	static std::future<bool> promise;
	static std::future<bool> delete_promise;
	static std::future<int> timeout_result;

	static bool show_window = true;
	static bool show_msg = false;
	static std::string feedback_msg = "";

	// Solo declaracion, definimos mas abajo
	static bool save_document(bool is_update);
	static bool delete_document();

	// No static porque se llama de otros archivos
	void reset() {
		// Resetea valores de esta clase
		id = NULL;
		monto_text = "";
		proposito = "";
		fecha = { 0 };
		fk_tipo_doc = 1;
		fk_categoria = 2;
	}

	static bool save_to_cloud() {
		bool result = false;
		if (id != NULL) {
			result = save_document(true);
		}
		if (id == NULL) {
			result = save_document(false);
		}
		return result;
	}

	// No static porque se llama de otros archivos
	void load_document(int id_doc) {
		// Nos pasan un id de Documento y cargamos sus datos en la clase
		Json::Value args;
		args["id"][0] = id_doc;
		args["sessionHash"] = AppState::sessionHash;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/documentos", args, "GET");

		// Deberia venir solo 1 resultado
		if (api_result.isArray()) {
			id = api_result[0]["id"].asInt();
			proposito = api_result[0]["proposito"].asString();
			// carga como numero/texto ya con separador de miles
			monto_text = FormatNumber::format(api_result[0]["monto"].asInt(), NULL, NULL);

			std::vector<std::string> t = Utilities::SplitString(api_result[0]["fecha"].asString(), "-");
			Utilities::SetTmDate(fecha, std::stoi(t.at(0)), std::stoi(t.at(1)), std::stoi(t.at(2)));

			// Carga tipoDoc y Categoria
			fk_tipo_doc = api_result[0]["fk_tipoDoc"].asInt();
			fk_categoria = api_result[0]["fk_categoria"].asInt();
		}
	}

	static bool save_document(bool is_update) {
		// Tomamos datos del formulario y convertimos en argument JSON
		// para hacer POST a la API
		Json::Value args;
		if (monto_text == "") {
			// No se puede monto vacio
			return false;
		}

		args["sessionHash"] = AppState::sessionHash;
		args["fk_tipoDoc"] = fk_tipo_doc;
		if (fk_tipo_doc != 1) {
			// Es algo diferente a Gasto entonces categoria va null
			args["fk_categoria"] = Json::Value::nullSingleton();
		}
		if (fk_tipo_doc == 1) {
			args["fk_categoria"] = fk_categoria;
		}
		args["proposito"] = proposito;
		int current_monto = std::stoi(Utilities::get_digits(monto_text, false));
		args["monto"] = current_monto;

		// Ajustamos formato de la fecha antes de enviar
		std::stringstream formatted_fecha;
		formatted_fecha << std::put_time(&fecha, "%Y-%m-%d");
		args["fecha"] = formatted_fecha.str();

		std::string method = "POST";
		// Si es una modificacion, modificamos metodo y añadimos
		// id que falta
		if (is_update) {
			args["id"] = id;
			method = "PUT";
		}

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/documentos", args, method);

		if (api_result.isMember("hasErrors")) {
			return false;
		}
		// Como la pantalla de documentos esta ejecutando render por estar abierta
		// llamamos funcion que asigna promesa que existe en ese archivo
		// y recarga los documentos en otro hilo
		Documentos::reload_docs();
		return true;
	}

	static bool delete_document() {
		Json::Value args;
		args["sessionHash"] = AppState::sessionHash;
		args["id"] = id;

		Json::Value api_result = ApiHelper::fn(AppState::apiPrefix + "/documentos", args, "DELETE");

		if (api_result.isMember("hasErrors")) {
			return false;
		}

		// Como la pantalla de documentos esta ejecutando render por estar abierta
		// llamamos funcion que asigna promesa que existe en ese archivo
		// y recarga los documentos en otro hilo
		Documentos::reload_docs();
		return true;
	}

	// render no puede ser static porque se referencia desde el Router
	// static hace la variable o clase privada
	void render() {
		// Variables se eliminan cuando render no esta activo
		// por no ser static ni ser globales. Deberia ayudar a liberar memoria
		const char* dateFormat = "%d/%m/%Y";

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

		std::string window_title = "Agregar Documento";
		if (id != NULL) {
			window_title = "Modificar Documento";
		}

		// Setea el tamaño de la ventana
		ImGui::SetNextWindowSize(ImVec2(350.0f, 300.0f));
		ImGui::SetNextWindowPos(
			ImVec2(
				ImGui::GetMainViewport()->Pos.x + 50,
				ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y / 2
			)
			, ImGuiCond_Appearing);

		ImGui::Begin(window_title.c_str(), &show_window, ImGuiWindowFlags_NoResize);

		// SI flag indica mostramos mensaje y setemamos otro hilo con
		// timeout para eliminar el mensaje
		if (show_msg) {
			ImGui::Text(feedback_msg.c_str());
			ImGui::Separator();
			ImGui::Spacing();
		}
		if (timeout_result._Is_ready()) {
			// Llamamos a Get para que se muera el hilo (deja de estar is_ready y C++ se encarga de matarlo?)
			timeout_result.get();
			show_msg = false;
			feedback_msg = "";
		}

		// Para que nos quede los botones queden centrados
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_SAVE, ImVec2(30.0f, 30.0f))) {
			// Pasamos en otro Hilo para no pegar el hilo de la renderizacion del la GUI
			promise = std::async(std::launch::async, save_to_cloud);
		}

		// Listener save_to_cloud
		if (promise._Is_ready()) {
			// Si es true tuvimos exito
			if (promise.get()) {
				// Mostramos Mensaje
				reset();
				feedback_msg = "Documento grabado correctamente " ICON_MD_SENTIMENT_VERY_SATISFIED;
				show_msg = true;
				timeout_result = TimerC::fn(3000, []() -> int {
					// Es funcion Lamnda no hace nada solo retorna cero
					// cuando se cumple el timeout
					// std::cout << "insidee timeout";
					return 0;
					});
			}
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_CLEAR_ALL, ImVec2(30.0f, 30.0f))) {
			reset();
		}

		// Solo mostramos Boton Eliminar si estamos editando
		if (id != NULL) {
			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_DELETE, ImVec2(30.0f, 30.0f))) {
				// Pasamos en otro Hilo para no pegar el hilo de la renderizacion del la GUI
				delete_promise = std::async(std::launch::async, delete_document);
			}
		}
		// Listener delete
		if (delete_promise._Is_ready()) {
			// Si es true tuvimos exito
			if (delete_promise.get()) {
				// Mostramos Mensaje
				show_msg = true;
				feedback_msg = "Documento eliminado correctamente " ICON_MD_MOOD;
				timeout_result = TimerC::fn(3000, []() -> int {
					// Es funcion Lamnda no hace nada solo retorna cero
					// cuando se cumple el timeout
					return 0;
					});
			}
		}

		ImGui::PopStyleVar();
		// Crea espacio en Blanco y linea separadora
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ==== Formulario ====
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