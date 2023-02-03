#include "AppState.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

#include "src/login/Login.h"
#include "src/documentos/documentos.h"
#include "src/Config.h"
#include "src/documentos/SingleDoc.h"
#include "src/graph/CategoryGraph.h";

namespace Router
{
	void router() {
		// Hace una verificacion, si sessionHash existe redirecciona a documentos inmediatamente
		if (!AppState::sessionHash.empty()) {
			AppState::route = "/documentos";
		}

		// ==== ROUTES ====
		if (AppState::route == "/login") {
			Login::login_render();
		}
		else if (AppState::route == "/documentos") {
			Documentos::render();
			// Si estamos en documentos siempre queremos mostrar el Grafico?
			CategoryGraph::render();
		}
		else {
			ImGui::Begin("Default Window");
			std::string msg = "No hay una ruta valida: " + AppState::route;
			ImGui::Text(msg.c_str());
			ImGui::End();
		}

		// En vez de flags podriamos crear 2 rutas. Asi se podrian tener hasta 2 ventanas abiertas al mismo tiempo
		// Con un flag diferente a la ruta podemos abrir y cerrar otras ventanas
		// sin cambiar la ruta
		if (AppState::showConfig) {
			Config::render();
		}
		if (AppState::showSingleDoc) {
			SingleDoc::render();
		}

	}
}