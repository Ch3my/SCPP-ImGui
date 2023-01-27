#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../AppState.h"

namespace Config
{
	void render() {
		// Creamos variable y le asignamos lo que este en el estado de la App
		// que ya deberia haber sido leido antes del mainLoop
		static std::string api_prefix = AppState::apiPrefix;

		// Utilizamos una propiedad que entrega ImGui
		// si pasamos show_window ImGui muestra una X en la ventana
		// que cuando se hace clic setea el valor a false, entonces cambiamos la navegacion
		// pero tambien tenemos que resetear show_window a true de nuevo sino despues no podemos
		// volver a abrir la ventana
		static bool show_window = true;
		if (!show_window) {
			AppState::showConfig = false;
			show_window = true;
		}

		// Setea el tamaño de la ventana
		ImGui::SetNextWindowSize(ImVec2(300.0f, 150.0f));
		ImGui::Begin("Config", &show_window, ImGuiWindowFlags_NoResize);
		ImGui::Text("Api Prefix");

		// Seteamos el largo del input
		ImGui::PushItemWidth(280.0f);
		ImGui::InputText("##apiprefixinput", &api_prefix);
		ImGui::PopItemWidth();

		if (ImGui::Button("Guardar")) {
			// Si hacen clic en el boton pasamos datos a estado y mandamos a grabar,
			// luego cambiamos la var en la ruta para que se cierre este ventana en el prox
			// ciclo del mainLoop
			AppState::apiPrefix = api_prefix;
			AppState::save_state_to_file();
			AppState::showConfig = false;
		}

		ImGui::End();
	}
}