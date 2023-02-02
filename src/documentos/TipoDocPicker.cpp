#include <imgui/imgui.h>
#include <json/json.h>

#include "../AppState.h"
#include "../helpers/ApiHelper.h"

#include <iostream>
#include <string>
#include <map>

namespace TipoDocPicker {

	// const char* porque es un string statico no se va a modificar
	// Deberia ser mejor performance
	static std::map<int, std::string> tipo_doc_list;
	static ImGuiComboFlags flags = 0;

	void load_tipo_doc() {
		ImGui::CheckboxFlags("ImGuiComboFlags_PopupAlignLeft", &flags, ImGuiComboFlags_PopupAlignLeft);


		Json::Value json_args;
		json_args["sessionHash"] = AppState::sessionHash;
		Json::Value data = ApiHelper::fn(AppState::apiPrefix + "/tipo-docs", json_args, "GET");

		for (Json::Value::ArrayIndex i = 0; i != data.size(); i++) {
			tipo_doc_list.insert({
				data[i]["id"].asInt(),
				data[i]["descripcion"].asString()
				});
		}
	}

	void render(int& tipo_doc) {
		if (tipo_doc_list.empty()) {
			load_tipo_doc();
		}

		// En este caso siempre queremos partir con Gasto seleccionado
		// tambien se puede acceder al primer item del map con tipo_doc_list.begin()->first
		std::string combo_holder = "";
		if (tipo_doc_list.count(tipo_doc)) {
			// at(key) nos devuelve el valor de la llave que especificamos
			// tambien se podria usar .find(tipo_doc)->fisrt/second que entrega llave y valor si necesitamos
			combo_holder = tipo_doc_list.at(tipo_doc);
		}

		if (ImGui::BeginCombo("##combo", combo_holder.c_str(), flags))
		{
			// ImGui::BeginCombo es true si el Combo esta abierto. Sino solo muestra lo que se definio
			// como preview_value
			for (auto const& [key, val] : tipo_doc_list)
			{
				const bool is_selected = (key == tipo_doc);
				if (ImGui::Selectable(val.c_str(), is_selected)) {
					tipo_doc = key;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void render(int& tipo_doc, bool& changed) {
		if (tipo_doc_list.empty()) {
			load_tipo_doc();
		}

		// En este caso siempre queremos partir con Gasto seleccionado
		// tambien se puede acceder al primer item del map con tipo_doc_list.begin()->first
		std::string combo_holder = "";
		if (tipo_doc_list.count(tipo_doc)) {
			// at(key) nos devuelve el valor de la llave que especificamos
			// tambien se podria usar .find(tipo_doc)->fisrt/second que entrega llave y valor si necesitamos
			combo_holder = tipo_doc_list.at(tipo_doc);
		}

		if (ImGui::BeginCombo("##combo", combo_holder.c_str(), flags))
		{
			// ImGui::BeginCombo es true si el Combo esta abierto. Sino solo muestra lo que se definio
			// como preview_value
			for (auto const& [key, val] : tipo_doc_list)
			{
				const bool is_selected = (key == tipo_doc);
				if (ImGui::Selectable(val.c_str(), is_selected)) {
					tipo_doc = key;
					changed = true;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}