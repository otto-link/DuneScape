// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.

#include <iostream>
#include <string>
#include <vector>

#include <omp.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/array.hpp"
#include "core/dunefield.hpp"

void array_to_texture(dunescape::Array &array,
                      GLuint           &image_texture,
                      int               colormap)
{
  glBindTexture(GL_TEXTURE_2D, image_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

  switch (colormap)
  {
  case 0:
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 array.shape[0],
                 array.shape[1],
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 array.to_img_8bit_grayscale().data());
    break;

  case 1:
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 array.shape[0],
                 array.shape[1],
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 array.to_img_8bit_nipy().data());
    break;
  }
}

static void glfw_error_callback(int error, const char *description)
{
  std::cout << "GLFW Error " << error << " " << description << std::endl;
}

int main()
{
  omp_set_num_threads(omp_get_max_threads() - 1);

  // --- Initialize dune field
  static int           width = 512;
  static int           height = 128;
  static int           h0 = 4;
  static int           seed = 1;
  std::vector<int>     shape = {width, height};
  dunescape::DuneField df = dunescape::DuneField(shape);

  df.seed = seed;
  df.h.randomize(0, h0, 1);
  df.update_shadow();

  GLuint image_texture = 0;
  glGenTextures(1, &image_texture); // to show dune field

  // --- ImGUI init
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  GLFWwindow *window =
      glfwCreateWindow(1500, 900, "DuneScape", nullptr, nullptr);
  if (window == nullptr)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  //
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool pause = false;

    static int cmap = 0;
    static int sub_iterations = 1;

    {
      ImGui::Begin("Settings");

      if (ImGui::Button("Reset"))
      {
        df.h.randomize(0, h0, df.seed);
        array_to_texture(df.h, image_texture, cmap);
      }

      ImGui::SameLine();
      {
        if (ImGui::Button("Export"))
          df.h.to_png("export.png");

        if (ImGui::IsItemHovered())
        {
          ImGui::BeginTooltip();
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextUnformatted("Saved to export.png");
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
        }
      }

      ImGui::SameLine();
      ImGui::Checkbox("Pause simulation", &pause);
      if (!pause)
        for (int it = 0; it < sub_iterations; it++)
        {
          df.cycle();
          array_to_texture(df.h, image_texture, cmap);
        }
      else
      {
        ImGui::SameLine();
        if (ImGui::Button("Next frame"))
        {
          df.cycle();
          array_to_texture(df.h, image_texture, cmap);
        }
      }

      ImGui::SliderInt("Width", &width, 32, 2048);
      ImGui::SliderInt("Height", &height, 32, 2048);

      if ((width != df.shape[0]) or (width != df.shape[1]))
      {
        width -= width % 32;
        height -= height % 32;
        df.set_shape({width, height});
        array_to_texture(df.h, image_texture, cmap);
      }

      ImGui::Spacing();
      ImGui::SeparatorText("Initialization");

      ImGui::SliderInt("Sand height", &h0, 1, 50);
      ImGui::DragInt("Seed", &seed);
      df.seed = (uint)seed;

      ImGui::Spacing();
      ImGui::SeparatorText("Parameters");

      ImGui::InputInt("Hop length", &df.hop_length);
      df.hop_length = std::max(1, df.hop_length);

      ImGui::SliderFloat("Pr. deposit bare", &df.prob_deposit_bare, 0.f, 1.f);
      ImGui::SliderFloat("Pr. sandy bare", &df.prob_deposit_sand, 0.f, 1.f);

      ImGui::Spacing();
      ImGui::SeparatorText("Preview");

      ImGui::Text("Colormap:");
      ImGui::SameLine();
      ImGui::RadioButton("Grayscale", &cmap, 0);
      ImGui::SameLine();
      ImGui::RadioButton("Nipy spectral", &cmap, 1);

      ImGui::InputInt("Sub-iterations (before render)", &sub_iterations);

      ImGui::End();
    }

    {
      ImGui::Begin("Dune field");

      ImVec2 win_size = ImGui::GetWindowSize();
      float  img_scaling =
          std::min(win_size[0] / df.shape[0], win_size[1] / df.shape[1]);
      ImVec2 img_size = {img_scaling * df.shape[0], img_scaling * df.shape[1]};

      ImGui::Image((void *)(intptr_t)image_texture, img_size);

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
                 clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
