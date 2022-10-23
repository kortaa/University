//===============================//
//==========Adam Korta===========//
//===============================//

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
//#include "global.h"
//#include "struct.h"

#define WINDOW_WIDTH (1024)
#define WINDOW_HEIGHT (600)

#define SPEED  (50);

int words_speed = 100;
int faster = 0;

SDL_Window* win;
SDL_Renderer* rend;

SDL_Color color = {255, 255, 255, 0};
SDL_Color input_color = { 181, 31, 109, 0};
SDL_Color score_color = { 255, 116, 45, 0 };


TTF_Font* font;
TTF_Font* score_font;

struct words_struct {


	SDL_Surface* text;
	SDL_Color color;
	SDL_Texture* tex;
	SDL_Rect dest;

	char word[20];

	float x_pos;
	float y_pos;

	float x_vel;
	float y_vel;
	bool flag;

};

struct img_struct {
	SDL_Surface* image;
	SDL_Texture* tex;
	SDL_Rect dest;
};

struct words_struct words[11];
struct words_struct input;
struct words_struct score[5];
struct img_struct hearts[3];
struct words_struct restart;
struct words_struct exiter;


char worder[1000][15];

//bool flags[10];

char c;

int random[10];

int score_i = 0;
int best_score;

int lifes = 2;

bool first = false;
bool close_requested = false; 

int x, y;

int next = 0;
int m = 0;
int temp_score = 1;

void destroy()
{
	for (int i = 0; i < 10; i++) words[i].tex = NULL;
	SDL_DestroyTexture(hearts->tex);
	SDL_DestroyTexture(input.tex);
	SDL_DestroyTexture(score->tex);
	SDL_DestroyTexture(restart.tex);
	SDL_DestroyTexture(exiter.tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

void save()
{
	FILE* f;

	f = fopen("best.txt", "w");

	if (!f)
	{
		printf("Error opening file");
		fclose(f);
		
	}

	fprintf(f,  "%d", best_score);

	fclose(f);

}

void rand_zero()
{
	for (int i = 0; i < 10; i++) random[i] = -1;
}

int sprawdzenie()
{
	for (int i = 0; i < 10; i++)
	{
		if (_strcmpi(input.word, words[i].word) == 0) return i;
	}

	return -1;
}

void zerowanie()
{
	for (int i = 0; i < 20; i++) input.word[i] = '\0';
}

bool exiter_button(int n)
{
	if (n == 0) exiter.color = color;
	else exiter.color = input_color;
	font = TTF_OpenFont("arial.ttf", 30);

	if (!font)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	exiter.text = TTF_RenderText_Solid(font, "Exit", exiter.color);

	if (!exiter.text)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	exiter.tex = SDL_CreateTextureFromSurface(rend, exiter.text);

	SDL_FreeSurface(exiter.text);

	if (!exiter.tex)
	{
		printf("error creating texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;

	}

	SDL_QueryTexture(exiter.tex, NULL, NULL, &exiter.dest.w, &exiter.dest.h);

	exiter.dest.x = (WINDOW_WIDTH - exiter.dest.w) / 2;
	exiter.dest.y = restart.dest.y + restart.dest.h + 50;
}

bool restart_button(int n)
{
	if (n == 0) restart.color = color;
	else restart.color = input_color;
	font = TTF_OpenFont("arial.ttf", 30);

	if (!font)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	restart.text = TTF_RenderText_Solid(font, "Restart", restart.color);

	if (!restart.text)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	restart.tex = SDL_CreateTextureFromSurface(rend, restart.text);

	SDL_FreeSurface(restart.text);

	if (!restart.tex)
	{
		printf("error creating texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;

	}

	SDL_QueryTexture(restart.tex, NULL, NULL, &restart.dest.w, &restart.dest.h);


	restart.dest.x = (WINDOW_WIDTH - restart.dest.w) / 2;
	restart.dest.y = score[2].dest.y + score[2].dest.h + 100;
}

bool best()
{
	FILE* f;

	f = fopen("best.txt", "r");

	if (!f)
	{
		printf("Error opening file");
		fclose(f);
		return false;
	}

	fscanf(f, "%d", &best_score);

	fclose(f);

	return true;


}

bool making_hearts()
{
	for (int i = 0; i < 3; i++)
	{
		hearts[i].image = IMG_Load("heart.jpg");

		if (!hearts[i].image)
		{
			printf("Error creating image: %s\n", SDL_GetError());
			SDL_DestroyRenderer(rend);
			SDL_DestroyWindow(win);
			SDL_Quit();
			return false;
		}

		hearts[i].tex = SDL_CreateTextureFromSurface(rend, hearts[i].image);
		SDL_FreeSurface(hearts[i].image);

		if (!hearts[i].tex)
		{
			printf("error creating texture: %s\n", SDL_GetError());
			SDL_DestroyRenderer(rend);
			SDL_DestroyWindow(win);
			SDL_Quit();
			return false;

		}

		//hearts[i].dest.h /= 100;
		hearts[i].dest.w /= 4;

		SDL_QueryTexture(hearts[i].tex, NULL, NULL, &hearts[i].dest.w, &hearts[i].dest.h);

		if (i == 0) hearts[i].dest.x = (WINDOW_WIDTH - 100);
		else hearts[i].dest.x = hearts[i - 1].dest.x - hearts[i - 1].dest.w;

		hearts[i].dest.y = 100;


	}
	return true;
}

bool making_score(int n)
{
	score[n].color = score_color;

	score_font = TTF_OpenFont("arial.ttf", 30);

	if (n == 1)_itoa(score_i, score[n].word, 10);
	else if (n == 0)strcpy(score[n].word, "SCORE");
	else if(n == 3)_itoa(best_score, score[n].word, 10);
	else if(n == 4)
	{ 
		strcpy(score[n].word, "NEW BEST SCORE!"); 
		score_font = TTF_OpenFont("arial.ttf", 50);
	}

	score[n].text = TTF_RenderText_Solid(score_font, score[n].word, score[n].color);

	if (!score[n].text)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	score[n].tex = SDL_CreateTextureFromSurface(rend, score[n].text);
	SDL_FreeSurface(score[n].text);

	if (!score[n].tex)
	{
		printf("error creating texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;

	}

	SDL_QueryTexture(score[n].tex, NULL, NULL, &score[n].dest.w, &score[n].dest.h);

	if (n == 0)
	{
		score[n].dest.x = (250 - score[n].dest.w) / 2;
		score[n].dest.y = 100;
	}

	else if(n == 1)
	{
		score[n].dest.x = (250 - score[n].dest.w) / 2;
		score[n].dest.y = score[n - 1].dest.y + score[n - 1].dest.h;
	}
	else if (n == 2)
	{
		score[n].dest.x = (250 - score[n].dest.w) / 2;
		score[n].dest.y = WINDOW_HEIGHT -  score[n].dest.w;
	}
	else if (n == 3)
	{
		score[n].dest.x = (250 - score[n].dest.w) / 2;
		score[n].dest.y = score[n - 1].dest.y + score[n - 1].dest.h;
	}
	else
	{
		score[n].dest.x = (WINDOW_WIDTH - score[n].dest.w) / 2;
		score[n].dest.y = (WINDOW_HEIGHT - score[n].dest.h) / 2;;
	}



	return true;

}

bool making_input()
{
	input.color = input_color;

	input.text = TTF_RenderText_Solid(font, input.word, input.color);

	if (!input.text)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	input.tex = SDL_CreateTextureFromSurface(rend, input.text);
	SDL_FreeSurface(input.text);

	if (!input.tex)
	{
		printf("error creating texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;

	}

	SDL_QueryTexture(input.tex, NULL, NULL, &input.dest.w, &input.dest.h);

	input.dest.x = (WINDOW_WIDTH - input.dest.w) / 2;
	input.dest.y = WINDOW_HEIGHT - 76;

	return true;
}

bool slowka()
{
	FILE* f;

	f = fopen("words.txt", "r");

	if (!f)
	{
		printf("Error opening file");
		fclose(f);
		return false;
	}

	for (int i = 0; i < 1000; i++) fscanf(f, "%s", worder[i]);

	fclose(f);
	
	return true;


}

bool creating_texture(int n)
{
	words[n].color = color;
	font = TTF_OpenFont("arial.ttf", 28);

	//strcpy_s(words[n].word, sizeof(words[n].word), "Nice");

	if (!font)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}
	srand(time(0));
	int temp = rand() % 1000;
	for (int i = 0; i < 10; i++)
	{
		if (temp == random[i])
		{
			temp = rand() % 1000;
			i = 0;
		}
	}
	random[n] = temp;
	strcpy(words[n].word, worder[random[n]]);
	words[n].text = TTF_RenderText_Solid(font, words[n].word, words[n].color);

	if (n == 9) rand_zero();

	if (!words[n].text)
	{
		printf("Error creating Text: %s\n", TTF_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	words[n].tex = SDL_CreateTextureFromSurface(rend, words[n].text);
	SDL_FreeSurface(words[n].text);

	if (!words[n].tex)
	{
		printf("error creating texture: %s\n", SDL_GetError());
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;

	}

	SDL_QueryTexture(words[n].tex, NULL, NULL, &words[n].dest.w, &words[n].dest.h);

	words[n].y_pos = 0;
	words[n].y_vel = SPEED;

	if (n % 4 == 0 || n % 4 == 2) words[n].dest.x = (WINDOW_WIDTH - words[n].dest.w) / 2;

	else if(n % 4 == 1) words[n].dest.x = (WINDOW_WIDTH - words[n].dest.w) / 2 + words[n - 1].dest.w;

	else if (n % 4 == 3)
	{
		words[n].dest.x = (WINDOW_WIDTH - words[n].dest.w) / 2 - words[n - 1].dest.w;
	}

	return true;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("Error initializing SDL: %s\n", SDL_GetError());
		return false;
	}

	if (TTF_Init() < 0)
	{
		printf("Error initializing TTF: %s\n", TTF_GetError());
		return false;
	}

	win = SDL_CreateWindow("Slowomania", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	if (!win)
	{
		printf("error creating SDL: %s\n", SDL_GetError());
		return false;
	}

	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	rend = SDL_CreateRenderer(win, -1, render_flags);

	if (!rend)
	{
		printf("error creating render: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return false;
	}

	for (int i = 0; i < 10; i++) words[i].flag = false;

	strcpy(score[2].word, "BEST SCORE");

	next = 0;
	m = 0;
	temp_score = 1;

	score_i = 0;

	words_speed = 100;
	faster = 0;

	rand_zero();

	if (!slowka()) return false;

	if (!best()) return false;

	for (int i = 0; i < 5; i++) if (!making_score(i)) return false;
	if (!making_hearts()) return false;

	zerowanie();

	return true;
}

void end()
{
	if (first == true)
	{
		strcpy(score[2].word, score[4].word);
		making_score(2);
		save();
	}

	for (int i = 0; i < 4; i++) score[i].dest.x = (WINDOW_WIDTH - score[i].dest.w) / 2;


	score[0].dest.y = 100;
	score[1].dest.y = score[0].dest.y + score[0].dest.h;

	score[2].dest.y = score[1].dest.y + score[1].dest.h + 100;
	score[3].dest.y = score[2].dest.y + score[2].dest.h;

	if (!restart_button(0)) close_requested = true;
	if (!exiter_button(0)) close_requested = true;

	while (!close_requested)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				destroy();
				close_requested = true;
				return;
			}

			if (event.type == SDL_MOUSEMOTION)
			{
				x = event.motion.x;
				y = event.motion.y;

				if (x >= restart.dest.x && x <= restart.dest.x + restart.dest.w && y >= restart.dest.y && y <= restart.dest.y + restart.dest.h)
				{
					if (!restart_button(1)) return;
				}
				else if (x >= exiter.dest.x && x <= exiter.dest.x + exiter.dest.w && y >= exiter.dest.y && y <= exiter.dest.y + exiter.dest.h)
				{
					if (!exiter_button(1)) return;
				}
				else
				{
					if (!restart_button(0)) return;
					if (!exiter_button(0)) return;
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (x >= restart.dest.x && x <= restart.dest.x + restart.dest.w && y >= restart.dest.y && y <= restart.dest.y + restart.dest.h)
				{
					destroy();
					return;
				}
				else if (x >= exiter.dest.x && x <= exiter.dest.x + exiter.dest.w && y >= exiter.dest.y && y <= exiter.dest.y + exiter.dest.h)
				{
					destroy();
					close_requested = true;
					return;
				}

			}
		}


		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);

		for (int i = 0; i < 4; i++) SDL_RenderCopy(rend, score[i].tex, NULL, &score[i].dest);

		SDL_RenderCopy(rend, restart.tex, NULL, &restart.dest);
		SDL_RenderCopy(rend, exiter.tex, NULL, &exiter.dest);

		SDL_RenderPresent(rend);

		SDL_Delay(1000 / 60);

	}
	//destroy();
}

void loop()
{
	if (!init()) return;
	lifes = 2;
	while (!close_requested)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				close_requested = true;
				destroy();
			}

			if (event.type == SDL_KEYDOWN)
			{
				if (event.type == SDL_KEYDOWN)
				{
					c = (char)event.key.keysym.sym;

					if (c == ' ')
					{

						input.tex = NULL;

						if (sprawdzenie() >= 0)
						{
							score_i += strlen(words[sprawdzenie()].word) * 10;

							if (!making_score(1)) return;

							words[sprawdzenie()].tex = NULL;

							if (score_i > best_score)
							{
								best_score = score_i;

								if (!making_score(3)) return;

								if (first == false)
								{
									first = true;
									for (int i = 0; i < 50; i++)
									{
										SDL_RenderClear(rend);
										SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
										SDL_RenderCopy(rend, score[4].tex, NULL, &score[4].dest);
										SDL_RenderPresent(rend);

										SDL_Delay(1000 / 60);
									}
								}
							}
						}
						else
						{
							score_i -= 10 * strlen(input.word);
							if (!making_score(1)) return;
						}

						zerowanie();
						m = 0;
					}
					else
					{

						input.word[m] = c;
						m++;
						if (!making_input()) return;
					}
				}
			}
		}


		for (int i = 0; i < 10; i++)
		{
			if (i * words_speed == next && words[i].flag == false)
			{
				creating_texture(i);
				words[i].flag = true;
			}
		}


		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);

		for (int i = 0; i < 3; i++) SDL_RenderCopy(rend, hearts[i].tex, NULL, &hearts[i].dest);

		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
		SDL_RenderDrawLine(rend, 250, 0, 250, WINDOW_HEIGHT);
		SDL_RenderDrawLine(rend, WINDOW_WIDTH - 250, 0, WINDOW_WIDTH - 250, WINDOW_HEIGHT);
		SDL_RenderDrawLine(rend, 250, WINDOW_HEIGHT - 100, WINDOW_WIDTH - 250, WINDOW_HEIGHT - 100);
		//SDL_RenderClear(rend);

		for (int i = 0; i < 4; i++) SDL_RenderCopy(rend, score[i].tex, NULL, &score[i].dest);

		SDL_RenderCopy(rend, input.tex, NULL, &input.dest);


		for (int i = 0; i < 10; i++)
		{
			if (words[i].flag == true)
			{
				words[i].y_pos += words[i].y_vel / (60 - faster);

				words[i].dest.y = (int)words[i].y_pos;

				SDL_RenderCopy(rend, words[i].tex, NULL, &words[i].dest);
			}

			if (words[i].y_pos >= WINDOW_HEIGHT - 100 - words[i].dest.h)
			{
				if (words[i].tex)
				{
					hearts[lifes].tex = NULL;
					lifes--;
					if (lifes < 0)
					{
						end();
						return;
					}
				}
				words[i].flag = false;
				words[i].tex = NULL;

			}
		}

		SDL_RenderPresent(rend);

		SDL_Delay(1000 / 60);
		next++;
		if (next == 10 * words_speed) next = 0;


		if (score_i >= 500 * temp_score)
		{
			faster++;
			words_speed -= 5;
			temp_score++;
		}


	}
}



int main(int argc, char* argv[])
{
	while (!close_requested)
	{
		loop();
	}
	return 0;
}