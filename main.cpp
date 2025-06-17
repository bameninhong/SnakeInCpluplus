#include <iostream>
#include <raylib.h>
#include <deque> 
#include <raymath.h>

using namespace std;

//Color plate
Color green = {173, 204, 96, 255};
Color darkGreen = {42, 51, 24, 255};


//Game states
enum gameState{
	titleScreen,
	gameActive,
	gameOver
};

//Grind config
int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

//elementInDeque helps us to dont spawn the food in the snake
bool elementInDeque(Vector2 elemnt, deque<Vector2> deque){
	for(unsigned int i = 0; i < deque.size(); i++){
		if(Vector2Equals(deque[i], elemnt)){
			return true;
		}
	}
	return false;
}

bool eventTriggered(double intervall){
	double currentTIme = GetTime();
	if(currentTIme - lastUpdateTime >= intervall){
		lastUpdateTime = currentTIme;
		return true;
	}
	return false;

}

//classes
class Snake{

public:
	deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
	Vector2 direction = {1, 0};
	bool addSegment = false;
	Vector2 nextDirection = {1, 0};
	Texture2D headTexture;

	Snake(){
		Image image = LoadImage("Assets/snake_head.png");
		headTexture = LoadTextureFromImage(image);
		UnloadImage(image);
	}

	~Snake(){
		UnloadTexture(headTexture);
	}

	void Draw(){
		for(unsigned int i = 0; i<body.size(); i++){
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
			DrawRectangleRounded(segment, 0.4, 6, darkGreen);
		}

		float snakeHeadx = body[0].x;
		float snakeHeady = body[0].y;

		float rotation = 0;
		if (direction.x == 1) rotation = 90;    // Right
		if (direction.x == -1) rotation = 270; // Left
		if (direction.y == -1) rotation = 0; // Up
		if (direction.y == 1) rotation = 180;   // Down
		Rectangle dest = {offset + snakeHeadx * cellSize + cellSize/2, 
                     offset + snakeHeady * cellSize + cellSize/2, 
                     (float)cellSize, (float)cellSize};
		Vector2 origin = {(float)cellSize/2, (float)cellSize/2};
		DrawTexturePro(headTexture, 
					Rectangle{0, 0, (float)headTexture.width, (float)headTexture.height},
					dest, 
					origin, 
					rotation, 
					WHITE);
	}


	void Update(){
		direction = nextDirection;
		body.push_front(Vector2Add(body[0], direction));
		if(addSegment == true){
			addSegment = false;
		}else{
			body.pop_back();
		}
	}


	//void ChangeDirection will prevent reversing Direction 
	void ChangeDirection(Vector2 newDirection){
		if(Vector2Length(Vector2Add(direction, newDirection)) !=0){
			nextDirection = newDirection;
		}
	} 

	void Reset(){
		body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
		direction = {1, 0};
	}

};

class Food{

public:
	Vector2 position;
	Texture2D texture;
	
	Food(deque<Vector2> snakeBody){
		Image image = LoadImage("Assets/apple_pixel.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}

	~Food(){
		UnloadTexture(texture);
	}
	
	void Draw(){
		DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
	}

	Vector2 GenerateRandomCell(){
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{x, y};
	}
	
	Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
		Vector2 position = GenerateRandomCell();
		while (elementInDeque(position, snakeBody))
		{
			position = GenerateRandomCell();
		}		
		return position; 
	}

};

class Game{
	public:
		gameState currentState = titleScreen;
		Snake snake = Snake();
		Food food = Food(snake.body);
		//bool running is needed for the pause function
		bool running = true;
		int score = 0;
		Music backgroundMusic;
		Sound eatSound;
		Sound gameOverSound;

		Game(){
			InitAudioDevice();
			backgroundMusic = LoadMusicStream("Sounds/background_music.mp3");
			SetMusicVolume(backgroundMusic, 0.5f);
			PlayMusicStream(backgroundMusic);
			eatSound = LoadSound("Sounds/apple_eating.mp3");
			gameOverSound = LoadSound("Sounds/gameover.mp3");
		}

		~Game(){
			UnloadMusicStream(backgroundMusic);
			UnloadSound(eatSound);
			UnloadSound(gameOverSound);
			CloseAudioDevice();
		}

		void Draw(){
			snake.Draw();
			food.Draw();
		};

		void Update(){
			if(running){
				snake.Update();
				CheckCollisionWithFood();
				CheckCollisionWithEdges();
				CheckCollisionWithTail();
			}
		};

		void UpdateMusic(){
			UpdateMusicStream(backgroundMusic);
		}

		void CheckCollisionWithFood(){
			if(Vector2Equals(snake.body[0], food.position)){
				
				food.position = food.GenerateRandomPos(snake.body);
				snake.addSegment = true;
				score ++;
				PlaySound(eatSound);
			}
		}

		void CheckCollisionWithEdges(){
			if(snake.body[0].x == cellCount || snake.body[0].x == -1){
				GameOver();
				PlaySound(gameOverSound);
			}
			if(snake.body[0].y == cellCount || snake.body[0].y == -1){
				GameOver();
				PlaySound(gameOverSound);
			}
		}

		void CheckCollisionWithTail(){
			deque<Vector2> headlessBody = snake.body;
			headlessBody.pop_front();
			if(elementInDeque(snake.body[0],headlessBody)){
				GameOver();
				PlaySound(gameOverSound);
			}
		}

		void GameOver(){
			snake.Reset();
			food.position = food.GenerateRandomPos(snake.body);
			running = false;
			score = 0;
			currentState = titleScreen;
		}

		void DrawTitleScreen() {
			// Title text
			const char* titleText = "SNAKE GAME";
			int titleFontSize = 50;
			int titleWidth = MeasureText(titleText, titleFontSize);
			DrawText(titleText, (GetScreenWidth() - titleWidth) / 2, offset + 100, titleFontSize, darkGreen);

			// Start button
			const char* buttonText = "START GAME";
			int buttonFontSize = 30;
			int buttonWidth = MeasureText(buttonText, buttonFontSize);
			Rectangle buttonRect = {
				(GetScreenWidth() - buttonWidth - 40) / 2.0f,
				GetScreenHeight() / 2.0f,
				(float)buttonWidth + 40,
				50
			};

			// Check if mouse is over button
			bool mouseOverButton = CheckCollisionPointRec(GetMousePosition(), buttonRect);

			// Draw button (changes color when hovered)
			DrawRectangleRec(buttonRect, mouseOverButton ? Color{100, 100, 100, 255} : darkGreen);
			DrawRectangleLinesEx(buttonRect, 2, RAYWHITE);
			DrawText(buttonText, (GetScreenWidth() - buttonWidth) / 2, GetScreenHeight() / 2 + 10, buttonFontSize, RAYWHITE);

			// Instructions
			const char* instrText = "Use arrow keys to move";
			int instrWidth = MeasureText(instrText, 20);
			DrawText(instrText, (GetScreenWidth() - instrWidth) / 2, GetScreenHeight() - 100, 20, darkGreen);
		}

};

int main(){
	
	std::cout<< "Starting the game\n";
	
	int width_height = cellSize * cellCount;
	int fps = 60;
	
	InitWindow(2*offset + width_height, 2*offset + width_height, "Snake");
	SetTargetFPS(fps);
	
	Game game = Game();
	
	
	//Gameloop
	while(WindowShouldClose() == false){
		BeginDrawing();
		ClearBackground(green);
		game.UpdateMusic();

		switch(game.currentState){
			case titleScreen:
				game.DrawTitleScreen();

				//Click checker
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					Vector2 mousePos = GetMousePosition();
					Rectangle buttonRect = {
						(GetScreenWidth() - 180) / 2.0f,
						GetScreenHeight() / 2.0f,
						180,
						50
                	};
                
					if (CheckCollisionPointRec(mousePos, buttonRect)) {
						game.currentState = gameActive;
					}
            	}
            
				// Also allow starting with Enter key
				if (IsKeyPressed(KEY_ENTER)) {
					game.currentState = gameActive;
				}

				break;
			case gameActive:
				if(eventTriggered(0.2) == true){
					game.Update();
				}

				if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
					game.snake.ChangeDirection({0, -1});
					game.running = true; 
				}
				if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
					game.snake.ChangeDirection({0, 1});
					game.running = true; 
				}
				if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
					game.snake.ChangeDirection({-1, 0});
					game.running = true; 
				}
				if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
					game.snake.ChangeDirection({1, 0});
					game.running = true;
				}
				if(IsKeyPressed(KEY_R)){
					game.running = true;
				}
				//Drawing
				DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)width_height+10, (float)width_height+10}, 5, darkGreen);
				game.Draw();
				DrawText("C++ Snake", offset, 20, 40, darkGreen);
				DrawText(TextFormat("Score : %i",game.score), cellCount*cellSize-2*offset , 20, 40, darkGreen);

				break;
		}
		
		EndDrawing();
	}
	
	
	
	
	CloseWindow();
	return 0;
}
