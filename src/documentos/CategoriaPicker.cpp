#include <imgui/imgui.h>
#include <json/json.h>

#include "../AppState.h"
#include "../helpers/ApiHelper.h"

#include <iostream>
#include <string>
#include <map>

namespace CategoriaPicker {

	// const char* porque es un string statico no se va a modificar
	// Deberia ser mejor performance
	static std::map<int, std::string> categorias_list;
	static ImGuiComboFlags flags = 0;

	void load_categorias() {
		ImGui::CheckboxFlags("ImGuiComboFlags_PopupAlignLeft", &flags, ImGuiComboFlags_PopupAlignLeft);
		

		Json::Value json_args;
		json_args["sessionHash"] = AppState::sessionHash;
		Json::Value data = ApiHelper::fn(AppState::apiPrefix + "/categorias", json_args, "GET");

		for (Json::Value::ArrayIndex i = 0; i != data.size(); i++) {
			categorias_list.insert({
				data[i]["id"].asInt(),
				data[i]["descripcion"].asString()
				});
		}
	}

	void render(int& categoria) {
		if (categorias_list.empty()) {
			load_categorias();
		}

		// Combo no tiene un value como select en HTML. asi que tendremos que invertir las llaves con los valores
		// para luego poder obtener el id por medio de la key

		// En este caso siempre queremos partir con Gasto seleccionado
		// tambien se puede acceder al primer item del map con categorias_list.begin()->first
		std::string combo_holder = "";
		if (categorias_list.count(categoria)) {
			// at(key) nos devuelve el valor de la llave que especificamos
			// tambien se podria usar .find(tipo_doc)->fisrt/second que entrega llave y valor si necesitamos
			combo_holder = categorias_list.at(categoria);
		}

		if (ImGui::BeginCombo("##combocategoria", combo_holder.c_str(), flags))
		{
			// ImGui::BeginCombo es true si el Combo esta abierto. Sino solo muestra lo que se definio
			// como preview_value
			for (auto const& [key, val] : categorias_list)
			{
				const bool is_selected = (key == categoria);
				if (ImGui::Selectable(val.c_str(), is_selected)) {
					categoria = key;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}