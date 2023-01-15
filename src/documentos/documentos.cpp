#include "../AppState.h"
#include "../src/helpers/ApiHelper.h"
#include "../src/helpers/FormatNumber.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>

#include <curl/curl.h>
#include <json/json.h>

// Define variables fuera de la clase para no estar definiendolas todo el rato o para conservar
// su valor entre renderizaciones. Se podria definir la variable como static dentro de la funcion que corresponda
static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
static Json::Value docs;
static ImVec2 cell_padding(5.0f, 5.0f);

namespace Documentos {
	// Tenemos que declarar (dentro del namespace) para llamar antes de definir, o dar vuelta las funciones
	Json::Value get_documentos();

	void render() {
		// Al setear la variable como static no se pierde entre renderizaciones

		// Aplica un stilo a la ventana, importante llamar antes del Begin
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
		ImGui::Begin("Documentos");

		if (ImGui::Button("Refresh")) {
			// TODO llamar get_documentos en otro hilo??
			docs = get_documentos();
		}

		// Crea la tabla con flags, IMPORTANTE, los flags definen si se pueden reodenar y todas esas vainas
		// IMPORTANTE. La creacion de la tabla va en un IF porque sino al collapse la ventana da error
		// seguramente porque la tabla no existe y esta tratando de definir sobre algo que no existe, entonces
		// el if sino existe la tabla se salta todo ese codigo y no da error :D
		if (ImGui::BeginTable("docs", 3, flags)) {
			// Crea la tabla y configura las columnas, hay mas flags que se podrian aplicar
			ImGui::TableSetupColumn("Fecha", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Proposito");
			ImGui::TableSetupColumn("Monto", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			// Primero hacemos loop en el array que no tiene nombre
			for (Json::Value::ArrayIndex i = 0; i != docs.size(); i++) {
				ImGui::TableNextRow();

				// Importante, los indices de columnas van desde 0, sino da error
				// seguramente por error de indice
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(docs[i]["fecha"].asString().c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Text(docs[i]["proposito"].asString().c_str());
				ImGui::TableSetColumnIndex(2);

				int monto = docs[i]["monto"].asInt();
				//ImGui::Text(FormatNumber::format(monto).c_str());

			}
			ImGui::EndTable();
		}
		ImGui::End();
		// Limpia un stilo a la ventana, importante llamar despues del END
		ImGui::PopStyleVar();
	}

	Json::Value get_documentos() {
		std::map<std::string, std::string> url_args;
		url_args.insert({ "fk_tipoDoc", "1" });
		url_args.insert({ "sessionHash","p0j13h6oockrrou5jfxlf" });

		std::string method = "POST";

		Json::Value data = ApiHelper::fn("http://localhost:3000/documentos", url_args, method);

		// TODO Formatear numeros aqui, hacerlo durante el runtime afecta los FPS reduciendo
		// un  50%, dicen que porque una funcion durante el render es demasiado lenta

		return data;
	}

}