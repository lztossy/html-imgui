#include "menu.h"
#include "imgui.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

std::map<int, float> slider_values;
std::string loadedHtmlContent;

std::string LoadHTMLFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "error: cannot open file " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void RenderInlineHTML(const std::string& html, int& unique_id) {
    size_t pos = 0;
    while (pos < html.size()) {
        if (html.compare(pos, 8, "<strong>") == 0) {
            pos += 8;
            size_t end = html.find("</strong>", pos);
            if (end != std::string::npos) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                ImGui::SameLine();
                ImGui::TextUnformatted(html.substr(pos, end - pos).c_str());
                ImGui::PopStyleColor();
                pos = end + 9;
            }
        }
        else if (html.compare(pos, 4, "<em>") == 0) {
            pos += 4;
            size_t end = html.find("</em>", pos);
            if (end != std::string::npos) {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
                ImGui::SameLine();
                ImGui::TextUnformatted(html.substr(pos, end - pos).c_str());
                ImGui::PopStyleVar();
                pos = end + 5;
            }
        }
        else if (html.compare(pos, 8, "<button>") == 0) {
            pos += 8;
            size_t end = html.find("</button>", pos);
            if (end != std::string::npos) {
                std::string buttonText = html.substr(pos, end - pos);
                ImGui::PushID(unique_id++);
                if (ImGui::Button(buttonText.c_str())) {
                    ImGui::Text("Button '%s' clicked!", buttonText.c_str());
                }
                ImGui::PopID();
                ImGui::SameLine();
                pos = end + 9;
            }
        }
        else if (html.compare(pos, 8, "<slider>") == 0) {
            pos += 8;
            size_t end = html.find("</slider>", pos);
            if (end != std::string::npos) {
                ImGui::PushID(unique_id);

                if (slider_values.find(unique_id) == slider_values.end()) {
                    slider_values[unique_id] = 0.0f;
                }

                ImGui::SliderFloat("", &slider_values[unique_id], 0.0f, 100.0f);

                ImGui::PopID();
                ImGui::SameLine();
                pos = end + 9;
                unique_id++;
            }
        }
        else if (html.compare(pos, 7, "<combo>") == 0) {
            pos += 7;  
            size_t end = html.find("</combo>", pos);
            if (end != std::string::npos) {
                std::string comboContent = html.substr(pos, end - pos);

                std::vector<std::string> options;
                size_t optionPos = 0;
                while ((optionPos = comboContent.find("|")) != std::string::npos) {
                    options.push_back(comboContent.substr(0, optionPos));
                    comboContent.erase(0, optionPos + 1);
                }
                options.push_back(comboContent);  

                static int current_option = 0;

                ImGui::PushID(unique_id++);  
                if (ImGui::BeginCombo("", options[current_option].c_str())) {
                    for (int i = 0; i < options.size(); i++) {
                        bool is_selected = (current_option == i);
                        if (ImGui::Selectable(options[i].c_str(), is_selected)) {
                            current_option = i;
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::PopID();  
                ImGui::SameLine();
                pos = end + 8;  
                unique_id++;  
            }
        }
        else {
            size_t end = html.find("<", pos);
            if (end == std::string::npos) {
                end = html.size();
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(html.substr(pos, end - pos).c_str());
            pos = end;
        }
    }
}

void RenderHTML(const std::string& html, int& unique_id) {
    size_t pos = 0;
    while (pos < html.size()) {
        if (html.compare(pos, 4, "<h1>") == 0) {
            pos += 4;
            size_t end = html.find("</h1>", pos);
            if (end != std::string::npos) {
                std::string text = html.substr(pos, end - pos);
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
                ImGui::TextWrapped("%s", text.c_str());
                ImGui::PopFont();
                pos = end + 5;
            }
        }
        else if (html.compare(pos, 3, "<p>") == 0) {
            pos += 3;
            size_t end = html.find("</p>", pos);
            if (end != std::string::npos) {
                std::string text = html.substr(pos, end - pos);
                ImGui::TextWrapped("");
                RenderInlineHTML(text, unique_id);
                pos = end + 4;
            }
        }
        else if (html.compare(pos, 5, "<div>") == 0) {
            pos += 5;
            size_t end = html.find("</div>", pos);
            if (end != std::string::npos) {
                std::string content = html.substr(pos, end - pos);

                if (!content.empty()) {
                    ImGui::PushID(unique_id++);
                    ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, 100);
                    ImGui::BeginChild(("box" + std::to_string(pos)).c_str(), child_size, true, ImGuiWindowFlags_None);

                    RenderInlineHTML(content, unique_id);

                    ImGui::EndChild();
                    ImGui::PopID();
                }
                pos = end + 6;
            }
        }
        else {
            size_t end = html.find("<", pos);
            if (end == std::string::npos) {
                end = html.size();
            }
            ImGui::TextWrapped("%s", html.substr(pos, end - pos).c_str());
            pos = end;
        }
    }
}

void Menu::Init() {
    ImGui::StyleColorsClassic();
}

void Menu::Render() {
    ImGui::Begin("HTML to ImGui Example");

    static char filePath[256] = "";  //add ur .html file here
    ImGui::InputText("HTML File Path", filePath, IM_ARRAYSIZE(filePath));

    if (ImGui::Button("Load HTML")) {
        loadedHtmlContent = LoadHTMLFromFile(filePath);
    }

    if (!loadedHtmlContent.empty()) {
        int unique_id = 0;
        RenderHTML(loadedHtmlContent, unique_id);  
    }
    else {
        ImGui::Text("No HTML content loaded.");
    }

    ImGui::End();
}
