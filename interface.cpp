#include <interface.h>

void RenderInterface()
{
  ImGui::SetNextWindowPos({10, 10});
  ImGui::SetNextWindowSize({300, 40});
  ImGui::Begin("interface0", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
  ImGui::Text("aceinetx.gzdoom (RECODE)");
  ImGui::End();

  ImGui::SetNextWindowPos({10, 55});
  ImGui::SetNextWindowSize({200, 40});
  ImGui::Begin("interface1", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
  ImGui::Text("%s", GZDoom::level->LevelName);
  ImGui::End();

  ImGui::SetNextWindowPos({210, 55});
  ImGui::SetNextWindowSize({100, 40});
  ImGui::Begin("interface2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
  ImGui::Text("Kills: %d/%d", GZDoom::level->KilledMonsters, GZDoom::level->TotalMonsters);
  ImGui::End();

  ImGui::SetNextWindowPos({310, 55});
  ImGui::SetNextWindowSize({100, 40});
  ImGui::Begin("interface3", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
  ImGui::Text("Items: %d/%d", GZDoom::level->FoundItems, GZDoom::level->TotalItems);
  ImGui::End();

  ImGui::SetNextWindowPos({410, 55});
  ImGui::SetNextWindowSize({120, 40});
  ImGui::Begin("interface4", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
  ImGui::Text("Secrets: %d/%d", GZDoom::level->FoundSecrets, GZDoom::level->TotalSecrets);
  ImGui::End();
}