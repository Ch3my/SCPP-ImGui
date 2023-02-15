#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_stdlib.h>
#include <curl/curl.h>
#include <json/json.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include "../AppState.h"
#include "../src/helpers/ApiHelper.h"
#include "../src/helpers/ApiHelperC.h"

namespace Login {
	void login_render() {
		ApiHelperC apiHelperC;

		static char username[64];
		static char pass[64];

		// Por defecto IMGUI solo usa char para el input, pero si se importa 
		//imgui/imgui_stdlib.h se puede usar std::string, se deja ejemplo de ambos
		// parece faltar algo porque al salir del input se vuelve a blanco, como sino
		// actualizara el largo del campo

		ImGui::Begin("Login");
		ImGui::Text("Username");
		// ## indica que solo es un label, no muestra el texto
		// sin ## aparece al lado derecho del input el texto
		ImGui::InputText("##usernameinput", username, IM_ARRAYSIZE(username));
		ImGui::Text("Pass");
		ImGui::InputText("##passinput", pass, IM_ARRAYSIZE(pass), ImGuiInputTextFlags_Password);

		bool btn = ImGui::Button("Login");
		if (btn) {
			// Llamar a la API y verificar que la clave corresponde
			Json::Value json_args;
			json_args["username"] = username;
			json_args["password"] = pass;
			
			Json::Value api_res = apiHelperC.fn(AppState::apiPrefix + "/login", json_args, "POST");
			//Json::Value api_res = ApiHelper::fn(AppState::apiPrefix + "/login", json_args, "POST");
			if (api_res.isMember("success")) {
				// Guardar en localstorage.json el sessionHash
				AppState::sessionHash = api_res["sessionHash"].asString();
				AppState::save_state_to_file();

				AppState::route = "/documentos";
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Config")) {
			AppState::showConfig = true;
		}

		ImGui::End();
	}

}