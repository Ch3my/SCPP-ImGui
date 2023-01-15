#include "AppState.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

#include "src/login/Login.h"
#include "src/documentos/documentos.h"

namespace Router
{
	void router() {
		if (AppState::route == "/login") {
			Login::login_render();
		}
		else if (AppState::route == "/documentos") {
			Documentos::render();
		}
		else {
			ImGui::Begin("Default Window");
			std::string msg = "No hay una ruta valida: " + AppState::route;
			ImGui::Text(msg.c_str());
			ImGui::End();
		}
	}
}