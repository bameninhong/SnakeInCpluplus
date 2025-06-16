#include <iostream>
#include <raylib.h>
#include <deque> 
#include <raymath.h>

using namespace std;

//Color plate
Color green = {173, 204, 96, 255};
Color darkGreen = {42, 51, 24, 255};


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

	void Draw(){
		for(unsigned int i = 0; i<body.size(); i++){
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
			DrawRectangleRounded(segment, 0.4, 6, darkGreen);
		}
	}


	void Update(){
		body.push_front(Vector2Add(body[0], direction));
		if(addSegment == true){
			addSegment = false;
		}else{
			body.pop_back();
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
		Snake snake = Snake();
		Food food = Food(snake.body);
		//bool running is needed for the pause function
		bool running = true;
		int score = 0;
		Sound eatSound;
		Sound gameOverSound;

		Game(){
			InitAudioDevice();
			eatSound = LoadSound("Sounds/apple_eating.mp3");
			gameOverSound = LoadSound("Sounds/gameover.mp3");
		}

		~Game(){
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

		if(eventTriggered(0.2) == true){
			game.Update();
		}

		if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
			game.snake.direction = {0, -1};
			game.running = true; 

		}
		if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
			game.snake.direction = {0, 1};
			game.running = true; 

		}
		if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
			game.snake.direction = {-1, 0};
			game.running = true; 
		}
		if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
			game.snake.direction = {1, 0};
			game.running = true;
		}
		if(IsKeyPressed(KEY_R)){
			game.running = true;
		}
		
		
		//Drawing
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)width_height+10, (float)width_height+10}, 5, darkGreen);
		game.Draw();
		DrawText("C++ Snake", offset, 20, 40, darkGreen);
		DrawText(TextFormat("Score : %i",game.score), cellCount*cellSize-2*offset , 20, 40, darkGreen);
		EndDrawing();
	}
	
	
	
	
	CloseWindow();
	return 0;
}
