/* Filename: main.cpp
 *
 */
#include "tools.hpp"

glm::mat4 model {1}, view {}, proj {}, MVP {};
GLint uniMVP = 0;

void init_buffers(void)
{
  glm::vec3 toUp(0.f, 1.f, 0.f);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat points[] = {
    // Coordinates        Color
      0.f, 0.f, 0.f,      1.0f, 1.0f, 0.0f,
  };
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  // Create VAO
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Specify layout of point data
  GLuint posAttrib = get_attrib_location("pos");

  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
    6 * sizeof(GLfloat), 0);

  // Set up transformation matrices
  uniMVP = get_uniform_location("MVP");

  view = glm::lookAt(glm::vec3(0.0f, 0.4f, 1.3f), 
        glm::vec3(0.0f, 0.0f, 0.0f), toUp),
  proj = glm::perspective(glm::radians(45.0f), 8.0f / 6.0f, 0.1f, 10.0f),

  glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
  glDisable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  return;
}

///----------------------------------------------------------------------------
///
void run_app(void)
{
  std::chrono::seconds oneSec(1);
  std::chrono::time_point<std::chrono::system_clock> t_start, t_now;
  t_start = std::chrono::high_resolution_clock::now();
  int
    fps  = 0,
    fpsT = 5000;

  std::string win_title {};
  while (!glfwWindowShouldClose(pWin))
  {
    fps++;
    t_now = std::chrono::high_resolution_clock::now();
     std::chrono::duration<double> fp_ms = t_now - t_start;
    auto time = static_cast<float>(fp_ms.count());
    model *= glm::rotate(0.5f/static_cast<float>(fpsT), glm::vec3(1,1,0));

    MVP = proj * view * model;
    glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVP));

    if(time >= 1)
    {
      t_start = std::chrono::high_resolution_clock::now();
      fpsT = fps;
      fps = 0;
    }
    win_title = "FPS: " + std::to_string(fpsT);
    glfwSetWindowTitle(pWin, win_title.c_str());

      // Render frame 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, 1);
    glfwSwapBuffers(pWin);
    glfwPollEvents();
  }

  glfwDestroyWindow(pWin);
  return;
}

//###
int main()
{
  try  {
    init_opengl_content();
    init_program();
    init_buffers();
    run_app();
  }  catch(std::exception & e) {
    std::cout << e.what() << '\n';;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cout << "FAILURE: undefined exception.\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
