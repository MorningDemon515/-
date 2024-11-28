#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

Graphics::Graphics(HWND hwnd)
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        MessageBoxW(hwnd,L"Failed to initialize GLAD",L"Engine Error!",MB_OK);
    }
    else
    {
        std::cout << "GLAD!"<<std::endl;
    }

    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

    float vertices[] = {
        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
             1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned char* data = stbi_load("my.jpg", &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

}

Graphics::~Graphics()
{

}

int  Graphics::Draw()
{
    ShaderFromFile* shader = new ShaderFromFile("vertex.txt", "fragment.txt");

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->Use();
    
    
    int model, view, proj;

    glm::mat4 gmodel = glm::mat4(1.0f);
    glm::mat4 gview = glm::mat4(1.0f);
    glm::mat4 gproj = glm::mat4(1.0f);

    glm::vec2 size = glm::vec2((float)width,(float)height);

    // gmodel = glm::rotate(gmodel, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    gview = glm::translate(gview, glm::vec3(0.0f, 0.0f, -3.0f));
    gproj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    model = shader->GetShaderSourceUniform("model");
    view = shader->GetShaderSourceUniform("view");
    proj = shader->GetShaderSourceUniform("projection");

    glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(gmodel));
    glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(gview));
    glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(gproj));
    

    /*
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    */
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    delete shader;
    return 0;

}