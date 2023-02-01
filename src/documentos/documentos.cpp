#include "../AppState.h"
#include "../src/helpers/ApiHelper.h"
#include "../src/helpers/FormatNumber.h"
#include "./SingleDoc.h";
#include "../helpers/IconsMaterialDesign.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <thread>
#include <future>

#include <curl/curl.h>
#include <json/json.h>

// Define variables fuera de la clase para no estar definiendolas todo el rato o para conservar
// su valor entre renderizaciones. Se podria definir la variable como static dentro de la funcion que corresponda
static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
static Json::Value docs;
static float CELL_PADDING_V = 7.0f;
static ImVec2 cell_padding(CELL_PADDING_V, CELL_PADDING_V);
static std::future<Json::Value> promise;

namespace Documentos {
	// Tenemos que declarar (dentro del namespace) para llamar antes de definir, o dar vuelta las funciones
	Json::Value get_documentos();

	void render() {
		// Al setear la variable como static no se pierde entre renderizaciones

		// Aplica un stilo a la ventana, importante llamar antes del Begin
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
		ImGui::Begin("Documentos");

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 1.4f));
		if (ImGui::Button(ICON_MD_REFRESH, ImVec2(30.0f, 30.0f))) {
			// Usamos std::async para llamar a la funcion, Disponible desde C++11
			// std::async con std::launch::async se asegura de ejecutar la funcion async, problablemente en otro thread
			// std::async se encarga de crear el thread o de usar uno que ya exista
			promise = std::async(std::launch::async, get_documentos);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_CLEAR_ALL, ImVec2(30.0f, 30.0f))) {
			docs.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_ADD_CIRCLE, ImVec2(30.0f, 30.0f))) {
			SingleDoc::reset(); // Resetamos antes por si acaso
			AppState::showSingleDoc = true;
		}
		ImGui::PopStyleVar();

		// Si la promesa esta ok usamos su resultado
		if (promise._Is_ready()) {
			// al llamar .get ya promesa deja de _Is_ready() asi no se 
			// ejecuta mas de una vez este codigo
			docs = promise.get();
		}

		// Crea la tabla con flags, IMPORTANTE, los flags definen si se pueden reodenar y todas esas vainas
		// IMPORTANTE. La creacion de la tabla va en un IF porque sino al collapse la ventana da error
		// seguramente porque la tabla no existe y esta tratando de definir sobre algo que no existe, entonces
		// el if sino existe la tabla se salta todo ese codigo y no da error :D
		if (ImGui::BeginTable("docs", 3, flags)) {
			// Crea la tabla y configura las columnas, hay mas flags que se podrian aplicar
			ImGui::TableSetupColumn("Fecha", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Proposito");
			ImGui::TableSetupColumn("Monto", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableHeadersRow();

			// Primero hacemos loop en el array que no tiene nombre
			for (Json::Value::ArrayIndex i = 0; i != docs.size(); i++) {
				ImGui::TableNextRow();
				// TODO. Change row Color on Hover. Parece que no hay manera de hacer esto
				// habra que inventar algo o verlo en actualizacion de ImGui

				
				// Importante, los indices de columnas van desde 0, sino da error
				// seguramente por error de indice
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(docs[i]["fecha"].asString().c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Text(docs[i]["proposito"].asString().c_str());
				if (ImGui::IsItemClicked()) {
					// Seteamos documento en funcion que busca el documento
					// y lo carga
					SingleDoc::load_document(docs[i]["id"].asInt());
					// ahora que ya deberia estar cargado marcamos para que se muestre la ventana
					AppState::showSingleDoc = true;
				}

				ImGui::TableSetColumnIndex(2);

				// Aparentemente no existe una manera de alinear el texto a la derecha, asi que inventamos este
				// hack que calcula el ancho del texto y el espacio que sobra le pone un espacio en blanco
				float textWidth = ImGui::CalcTextSize(docs[i]["montoFormatted"].asString().c_str()).x;
				// Dummy crea un espacio en blanco con el alto y ancho que se especifica
				// GetColumnWidth obtiene el width de la columna en la tabla
				// GetColumnWidth no considera el Padding y queda parte del texto afuera
				// Debemos considerar en el calculo
				// Aparentemente ejecutar estos calculos aqui no afecta el rendimiento
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - textWidth - CELL_PADDING_V * 2, 0.0f));
				ImGui::SameLine(); // Para que ambos elementos queden en la misma linea, sino no se ve el espacio
				ImGui::Text(docs[i]["montoFormatted"].asString().c_str());
			}
			ImGui::EndTable();
		}
		ImGui::End();
		// Limpia un stilo a la ventana, importante llamar despues del END
		ImGui::PopStyleVar();
	}

	Json::Value get_documentos() {
		Json::Value json_args;
		json_args["fk_tipoDoc"] = 1;
		json_args["sessionHash"] = AppState::sessionHash;
		Json::Value data = ApiHelper::fn(AppState::apiPrefix + "/documentos", json_args, "GET");

		// isMember crash si tratamos de llamarlo sobre un array
		// por eso verificamos que sea objeto primero
		if (data.isObject() && data.isMember("hasErrors")) {
			// Si hay errores no podemos iterar, debemos mostrar mensaje y salir
			return NULL;
		}

		// Formatear numeros aqui, hacerlo durante el runtime afecta los FPS reduciendo
		// un  50%, dicen que porque una funcion durante el render es demasiado lenta
		for (Json::Value::ArrayIndex i = 0; i != data.size(); i++) {
			std::string formattedNumber = FormatNumber::format(data[i]["monto"].asInt(), NULL, NULL);
			data[i]["montoFormatted"] = formattedNumber;
		}

		return data;
	}
}