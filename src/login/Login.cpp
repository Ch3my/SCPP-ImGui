#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_stdlib.h>
#include <curl/curl.h>

#include <stdio.h>
#include <string>
#include "../AppState.h"
#include "../src/helpers/ApiHelper.h"

namespace Login {
	void login_render() {
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
			// comparar char con "" no funcionaba, se utilizo strcmp seguramente
			// es algo con el boxing o el largo del item

			// Llamar a la API y verificar que la clave corresponde
			if (strcmp(username, "admin") == 0 && strcmp(pass, "admin") == 0) {
				AppState::route = "/documentos";
			}
		}

		ImGui::End();
	}

}