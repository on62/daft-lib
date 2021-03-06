#include "gl_tools.hpp"

GLuint hdl_VAO;
GLuint pos_Buf;
GLuint col_Buf;

//----------------------------------------------------------------------------
const GLchar * vert_shader_array[] = {"#version 430\n\
                                                   \n\
layout (location=0) in vec3 VertexPosition;        \n\
layout (location=1) in vec3 VertexColor;           \n\
                                                   \n\
out vec3 FrColor;                                  \n\
                                                   \n\
void main()                                        \n\
{                                                  \n\
  FrColor  = VertexColor;                          \n\
  gl_Position = vec4(VertexPosition, 1.0);         \n\
}"};//------------------------------------------------------------------------
const GLchar * frag_shader_array[] = {"#version 430\n\
                                                   \n\
in vec3 FrColor;                                   \n\
out vec3 Color;                                    \n\
                                                   \n\
void main()                                        \n\
{                                                  \n\
  Color = FrColor;                                 \n\
}"};//------------------------------------------------------------------------

//## компиялция шейдера с контролем результата
void compile_shader(GLuint shader)
{
  glCompileShader(shader);
  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if(GL_FALSE == result)
  {
    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
    if(logLen)
    {
      char * log = new char[logLen];
      GLsizei written;
      glGetShaderInfoLog(shader, logLen, &written, log);
      std::cout << log << '\n';
      delete [] log;
    }
    ERR("Error compile SHADER");
  }
  return;
}

//### Компиляция и запуск GLSL программы
void gl_init_program(void)
{
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  if (!vertShader) ERR("Error create GL_VERTEX_SHADER");
  glShaderSource(vertShader, 1, vert_shader_array, NULL);
  compile_shader(vertShader);

  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (!fragShader) ERR("Error create GL_FRAGMENT_SHADER");
  glShaderSource(fragShader, 1, frag_shader_array, NULL);
  compile_shader(fragShader);

  GLuint program = glCreateProgram();
  if (!program) ERR("Error creating GLSL program\n");
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (GL_FALSE == status)
  {
    GLint logLen;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen)
    {
      char * log = new char[logLen];
      GLsizei written;
      glGetProgramInfoLog(program, logLen, &written, log);
      std::cout << log << '\n';
      delete [] log;
    }
    ERR("Failed to link GLSL program.\n");
  }
  glUseProgram(program);

  return;
}

//### Загрузка данных, установка начальных параметров
void gl_init_scene(void)
{
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClearColor(0.5f, 0.69f, 1.0f, 1.0f);

  float pos_Data[] = {
    -0.8f, -0.8f, 0.0f,
     0.8f, -0.8f, 0.0f,
     0.0f,  0.8f, 0.0f,
  };

  float col_Data[] = {
    0.4f, 1.0f, 0.4f,
    0.4f, 1.0f, 0.4f,
    0.4f, 1.0f, 0.4f,
  };

  glGenVertexArrays(1, &hdl_VAO);
  glBindVertexArray(hdl_VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &pos_Buf);
  glBindBuffer(GL_ARRAY_BUFFER, pos_Buf);
  glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), pos_Data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &col_Buf);
  glBindBuffer(GL_ARRAY_BUFFER, col_Buf);
  glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), col_Data, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindVertexArray(0);

  return;
}

void gl_draw_arrays(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindVertexArray(hdl_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  return;
}

