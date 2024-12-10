#include "Shader.h"

Graphics::Graphics(HWND hwnd)
{
    hh = hwnd;

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        MessageBoxW(hh,L"Failed to initialize GLAD",L"Engine Error!",MB_OK);
    }
    else
    {
        std::cout << "GLAD!"<<std::endl;
    }
    
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

}

Graphics::~Graphics()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Graphics::RenderText(std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    
    // Compile and setup the shader
    ShaderFromFile shader("vertex.txt", "fragment.txt");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WINDOW_WIDTH), 0.0f, static_cast<GLfloat>(WINDOW_HEIGHT));
    shader.Use();
    glUniformMatrix4fv(shader.GetShaderSourceUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        MessageBoxW(hh, L"Could not init FreeType Library", L"Engine Error!", MB_OK);

    if (FT_New_Face(ft, "C:/Windows/Fonts/simfang.ttf", 0, &face))
        MessageBoxW(hh, L"Failed to load font", L"Engine Error!", MB_OK);

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (wchar_t c : text) { // Use the actual text characters
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters[c] = character; // Use the actual character as the key
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Activate corresponding render state	
    shader.Use();
    glUniform3f(shader.GetShaderSourceUniform("textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    //std::string::const_iterator c;
    //for (c = text.begin(); c != text.end(); c++)
    for (wchar_t c : text)
    {
        auto it = Characters.find(c);
        if (it == Characters.end()) {
            continue; // Skip characters that are not loaded
        }

        Character ch = it->second;

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int  Graphics::Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            run = false;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int px = event.button.x;
                int py = event.button.y;

                int xx, yy, ww, hh;

                xx = 640 - 30 - 90;
                yy = 480 - 50;
                ww = 100;
                hh = 30;

                if (px >= xx && px <= xx + ww && py >= yy && py <= yy + hh)
                {
                    run = false;
                }
                
                
            }
            

            break;

        case SDL_KEYDOWN:

            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                run = false;
                break;

            case SDLK_w:

                break;

            default:
                break;
            }
  
            break;

        default:
            break;
        }

    }

    

    //RenderText(L"开始游戏", (float)1280/2 - 100, (float)720/2, 1.0f, glm::vec3(1.0, 1.0f, 1.0f));
    RenderText(L"退出", (float)(640 - 30 - 90), (float)(480 - 450), 1.0f, glm::vec3(1.0, 1.0f, 1.0f));

    return 0;

}

bool Graphics::IsDown(SDL_Rect rectt)
{
    int x = event.button.x;
    int y = event.button.y;

    return x >= rectt.x && x <= rectt.x + rectt.w &&
        y >= rectt.y && y <= rectt.y + rectt.h;
}