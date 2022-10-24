#include "game_logic.h";

// Initializes the Game Logic
GameLogic::GameLogic(GLFWwindow* window, float screenWidth, float screenHeight) {
    this->window = window;
    this->width = screenWidth;
    this->height = screenHeight;
	this->gameState = GameState::start;

	// compile and setup the shader
	// ----------------------------
	this->shader = new Shader("shader/text.vs", "shader/text.fs");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));
	this->shader->use();
	glUniformMatrix4fv(glGetUniformLocation(this->shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
   
    buildFreeText();
}

// Generates the font being used for the UI later
int GameLogic::buildFreeText() {
    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = "fonts/BebasNeue Bold.otf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
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
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Updates every frame (using main.cpp's while loop)
void GameLogic::update() {
    timer();
    drawUI();
}

// Decrements the game's timer, but only if it's active
void GameLogic::timer() {
	// Don't bother if we aren't playing
	if (gameState != GameState::playing)
		return;

	float dt = glfwGetTime() - startFrameTime;
	timeLeft = (timeLimit - dt);


	// We've reached the end
	if (timeLeft < 0)
		endGame();
}



// Start the game
void GameLogic::startGame() {
    if (this->gameState == GameState::start)
        playGame();
}

// Set variables to play the game
void GameLogic::playGame() {
    this->gameState = GameState::playing;
    this->points = 0;
    this->timeLimit = 150;
    this->timeLeft = timeLimit;
    startFrameTime = glfwGetTime();         // Keep track of starting time to subtract from glfwGetTime()
    this->level = 1;

}

// End the game
void GameLogic::endGame() {
	this->gameState = GameState::end;
}

// Retry the game after it ended
void GameLogic::retryGame() {
    if (this->gameState == GameState::end)
        playGame();
}

// Adds points to total amount
void GameLogic::gainPoints(int amt) {
	this->points += amt;
}

// Adds levels to total amount
void GameLogic::nextLevel(int amt) {
    this->level += amt;
}

// Checks if user is currently playing or not (starting or ending a game)
bool GameLogic::isPlaying() {
    return (this->gameState == GameState::playing);
}

// Draw the game's UI (Score, timer, and messages)
void GameLogic::drawUI() {

    // Waiting to Play
    if (this->gameState == GameState::start) {
        renderText("Press", this->width / 2 -170, this->height/2 - 40, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderText("SPACE", this->width / 2 -70, this->height / 2 - 40, 1.0f, glm::vec3(0.898f, 0.545f, 0.243f));
        renderText("to play.", this->width / 2 +30, this->height / 2 - 40, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

          renderText("Use", this->width / 2 -220, this->height/3, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderText("ARROW KEYS", this->width / 2 -150, this->height / 3, 1.0f, glm::vec3(0.898f, 0.545f, 0.243f));
        renderText("to rotate.", this->width / 2 +50, this->height / 3, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    }
    // Playing/Ending
    else {
        int sec = (int)ceil(timeLeft) % 60;
        int min = (int)ceil(timeLeft) / 60;
        int pointConter = (int)ceil(points);
        //float hr = (int)timeLeft / 3600;
        //cout << timeLeft << " -----> " << hr << ":" << min << ":" << sec << "  ||||  " << "Points: " << points << endl;
        
        // Add a 0 before seconds if the timer only has 1 digit
        string timeSec;
        if (sec < 10)
            timeSec = "0" + std::to_string(sec);
        else
            timeSec = std::to_string(sec);

        renderText("POINTS:", 10.0f, this->height - 50, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        renderText(std::to_string(points * level), 130.0f, this->height - 50, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        renderText("TIME:", this->width - 170, this->height - 50, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        renderText(std::to_string(min) + ":" + timeSec, this->width - 80, this->height - 50, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        renderText("LEVEL:", this->width/2 - 50, 30, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        renderText(std::to_string(level), this->width/2 + 60, 30, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));



        //renderText("LEVEL:", 10.0f, this->height - 660, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        //renderText(std::to_string(points), 130.0f, this->height - 660, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
       


        // GAME OVER
        if (this->gameState == GameState::end) {
            renderText("GAME OVER", this->width / 2 - 170, this->height / 2, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            renderText("Press", this->width / 2 - 140, this->height / 2 - 40, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderText("R", this->width / 2 - 40, this->height / 2 - 40, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            renderText("to retry.", this->width / 2 -10, this->height / 2 - 40, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
}


// Renders the given text using shaders
void GameLogic::renderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    this->shader->use();
    glUniform3f(glGetUniformLocation(this->shader->ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
