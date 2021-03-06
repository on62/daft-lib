#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>
#include <vector>
#include <fstream>
#include <memory>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#include "shaders.h"

#define POINTER(i) reinterpret_cast<const GLvoid*>(i)
GLuint hdl_vao;

static const struct
{
    float x, y, u, v;
} vertices[4] = {
    {-0.9f,-0.9f, 0.f, 1.f },
    { 0.9f,-0.9f, 1.f, 1.f },
    {-0.9f, 0.9f, 0.f, 0.f },
    { 0.9f, 0.9f, 1.f, 0.f }
};

struct color { unsigned char r, g, b, a; };
static const int attr_count = 4;

static void error_callback(int, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

inline int i_mul_fi(float f, int i) { return static_cast<int>(f * static_cast<float>(i)); }
inline int i_mul_if(int i, float f) { return i_mul_fi(f, i); }


void load_font(std::vector<unsigned char>& Buffer)
{
  // load font file
  std::ifstream file ("../font/DejaVu Sans Mono for Powerline.ttf", std::ifstream::binary);
  if (!file) fprintf(stderr, "error: not found font file");
  file.seekg (0, file.end);    // get length of file
  auto length = file.tellg();
  file.seekg (0, file.beg);
  std::vector<char> buffer(static_cast<size_t>(length), '\0');
  file.read (buffer.data(),length); // read data as a block
  if (!file) fprintf(stderr, "error: only %lld could be read", file.gcount());
  file.close();
  Buffer.resize(static_cast<size_t>(length), '\0');
  memmove(Buffer.data(), buffer.data(), Buffer.size());
}

unsigned char* get_font_buffer(std::vector<unsigned char>& Buffer)
{
  if(Buffer.empty()) load_font(Buffer);
  return Buffer.data();
}


int main(int, const char**)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 0);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    window = glfwCreateWindow(480, 480, "", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    if(!gladLoadGL()) { exit(-1); }

    char title[16] = {0};
    sprintf(title, "OpenGL ver. %d.%d", GLVersion.major, GLVersion.minor);
    glfwSetWindowTitle(window, title);
    glfwSwapInterval(1);

    // Настройка текстуры
    GLint tex_width = 400;
    GLint tex_height = 400;

    stbtt_fontinfo font_info {};
    std::vector<unsigned char> FnBuffer {};
    if (!stbtt_InitFont(&font_info, get_font_buffer(FnBuffer), 0)) fprintf(stderr, "failed\n");

    int b_w = tex_width; // bitmap width
    int b_h = tex_height; // bitmap height
    float l_h = 128.0f; // line height

    int word[] = { 0x0416, 0x0418, 0x0412, 0x0423, '!' };
    size_t w_len = sizeof(word)/sizeof(word[0]);

    // create a bitmap for the phrase
    std::vector<unsigned char> bitmap( static_cast<size_t>(b_w * b_h), '\0' );

    // calculate font scaling
    float scale = stbtt_ScaleForPixelHeight(&font_info, l_h);

    int left_dist = 50;  // отступ слева
    int top_dist = 120;  // отступ сверху

    int x = left_dist;

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &lineGap);

    ascent = static_cast<int>(static_cast<float>(ascent) * scale);
    descent = static_cast<int>(static_cast<float>(descent) * scale);

    for (size_t i = 0; i < w_len; ++i)
    {
     // how wide is this character
     int ax;
     int lsb;
     stbtt_GetCodepointHMetrics(&font_info, word[i], &ax, &lsb);

     /// get bounding box for character (may be offset to account for chars
     // that dip above or below the line
     int c_x1, c_y1, c_x2, c_y2;
     stbtt_GetCodepointBitmapBox(&font_info, word[i], scale, scale, &c_x1,
         &c_y1, &c_x2, &c_y2);

     // compute y (different characters have different heights
     int y = top_dist + ascent + c_y1;

     // render character (stride and offset is important here)
     int byteOffset = x + i_mul_if(lsb, scale) + (y * b_w);
     stbtt_MakeCodepointBitmap(&font_info, bitmap.data() + byteOffset,
         c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

     // advance x
     x += i_mul_if(ax, scale);

     // add kerning
     int kern;
     kern = stbtt_GetCodepointKernAdvance(&font_info, word[i], word[i + 1]);
     x += i_mul_if(kern, scale);
    }

    unsigned int image_size = static_cast<unsigned int>(tex_width * tex_height);
    std::vector<color> vImage(image_size, {255, 255, 255, 255});
    auto max = bitmap.size();
    for(unsigned int i = 0; i < max; ++i)
    {
      auto n = bitmap[i];
      if(n > 0)
      {
        n = static_cast<unsigned char>(255 - n);
        vImage[i].r = n;
        vImage[i].g = n;
        vImage[i].b = n;
      }
    }

    size_t texture_size = image_size * 4;
    std::vector<unsigned char> texture(texture_size, 255);
    memcpy(texture.data(), vImage.data(), texture_size);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    glActiveTexture(GL_TEXTURE0);
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height,
             0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint mvp_location  = glGetUniformLocation(program, "MVP");
    GLint vpos_location = glGetAttribLocation (program, "vPos");
    GLint t2d_location  = glGetAttribLocation (program, "texture_2dcoord");

    glGenVertexArrays(1, &hdl_vao);
    glBindVertexArray(hdl_vao);

    glEnableVertexAttribArray(static_cast<GLuint>(vpos_location));
    glVertexAttribPointer(static_cast<GLuint>(vpos_location), 2, GL_FLOAT,
      GL_FALSE, sizeof(float) * attr_count, POINTER(0));

    glEnableVertexAttribArray(static_cast<GLuint>(t2d_location));
    glVertexAttribPointer(static_cast<GLuint>(t2d_location), 2, GL_FLOAT,
      GL_FALSE, sizeof(float) * attr_count, POINTER(sizeof(float) * 2));

    glClearColor(.3f, .3f, .4f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = static_cast<float>(width)/static_cast<float>(height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, static_cast<GLfloat>(glfwGetTime()));
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, mvp[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
