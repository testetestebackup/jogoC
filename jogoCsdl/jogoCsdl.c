#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 50
#define GRID_WIDTH (SCREEN_WIDTH / GRID_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / GRID_SIZE)

#define TELA_INICIAL 0
#define TELA_JOGO 1
#define TELA_GAMEOVER 2

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[GRID_WIDTH * GRID_HEIGHT];
    int length;
    Point direction;
} Snake;

typedef struct {
    Point position;
} Food;

// Globais
int pontos = 0; // Contador de pontos
int estadoJogo = TELA_INICIAL;
TTF_Font* font; // Variável global para armazenar a fonte
SDL_Rect pontuacaoRect;

void initSnake(Snake* snake) {
    snake->length = 1;
    snake->body[0].x = GRID_WIDTH / 2;
    snake->body[0].y = GRID_HEIGHT / 2;
    snake->direction.x = 1;
    snake->direction.y = 0;
}

void initFood(Food* food, Snake* snake) {
    int valid = 0;
    while (!valid) {
        food->position.x = rand() % GRID_WIDTH;
        food->position.y = rand() % GRID_HEIGHT;
        valid = 1;
        for (int i = 0; i < snake->length; i++) {
            if (snake->body[i].x == food->position.x && snake->body[i].y == food->position.y) {
                valid = 0;
                break;
            }
        }
    }
}

void moveSnake(Snake* snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0].x += snake->direction.x;
    snake->body[0].y += snake->direction.y;

    if (snake->body[0].x < 0) snake->body[0].x = GRID_WIDTH - 1;
    if (snake->body[0].x >= GRID_WIDTH) snake->body[0].x = 0;
    if (snake->body[0].y < 0) snake->body[0].y = GRID_HEIGHT - 1;
    if (snake->body[0].y >= GRID_HEIGHT) snake->body[0].y = 0;
}

int checkCollision(Snake* snake) {
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erro ao iniciar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Inicializar a SDL_ttf
    if (TTF_Init() != 0) {
        printf("Erro ao iniciar SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* janela = SDL_CreateWindow("Jogo da Cobrinha", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (janela == NULL) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
    if (renderizador == NULL) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(janela);
        SDL_Quit();
        return 1;
    }

    // Carregar a fonte
    font = TTF_OpenFont("SLC_.ttf", 24);
    if (font == NULL) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(janela);
        SDL_Quit();
        return 1;
    }

    Snake snake;
    initSnake(&snake);

    Food food;
    initFood(&food, &snake);

    int jogoRodando = 1;
    SDL_Event e;

    SDL_Color corBranca = { 255, 255, 255, 255 };

    while (jogoRodando) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                jogoRodando = 0;
            }
            if (e.type == SDL_KEYDOWN) {
                if (estadoJogo == TELA_INICIAL) {
                    estadoJogo = TELA_JOGO;
                }
                else if (estadoJogo == TELA_GAMEOVER && e.key.keysym.sym == SDLK_r) {
                    estadoJogo = TELA_JOGO;
                    pontos = 0; // Reiniciar a contagem de pontos
                    initSnake(&snake); // Reiniciar a cobra
                    initFood(&food, &snake); // Reiniciar a comida
                }
                else {
                    switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.direction.y == 0) {
                            snake.direction.x = 0;
                            snake.direction.y = -1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (snake.direction.y == 0) {
                            snake.direction.x = 0;
                            snake.direction.y = 1;
                        }
                        break;
                    case SDLK_LEFT:
                        if (snake.direction.x == 0) {
                            snake.direction.x = -1;
                            snake.direction.y = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snake.direction.x == 0) {
                            snake.direction.x = 1;
                            snake.direction.y = 0;
                        }
                        break;
                    }
                }
            }
        }

        if (estadoJogo == TELA_JOGO) {
            moveSnake(&snake);

            if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
                snake.length++;
                pontos++; // Incrementa o contador de pontos
                initFood(&food, &snake);
            }

            if (checkCollision(&snake)) {
                estadoJogo = TELA_GAMEOVER;
            }

            // Limpar a tela com cor preta
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderClear(renderizador);

            // Desenhar a cobra com cor
            SDL_SetRenderDrawColor(renderizador, 0, 255, 0, 255);
            for (int i = 0; i < snake.length; i++) {
                SDL_Rect retangulo = { snake.body[i].x * GRID_SIZE, snake.body[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
                SDL_RenderFillRect(renderizador, &retangulo);
            }

            // Desenhar a comida com cor
            SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
            SDL_Rect foodRect = { food.position.x * GRID_SIZE, food.position.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
            SDL_RenderFillRect(renderizador, &foodRect);

            // Exibir o contador de pontos
            char texto[20];
            sprintf_s(texto, 20, "Pontos: %d", pontos);
            SDL_Surface* surface = TTF_RenderText_Solid(font, texto, corBranca);
            if (!surface) {
                printf("Erro ao criar superfície de texto: %s\n", TTF_GetError());
                return 1;
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderizador, surface);
            if (!texture) {
                printf("Erro ao criar textura de texto: %s\n", SDL_GetError());
                SDL_FreeSurface(surface);
                return 1;
            }
            SDL_FreeSurface(surface);

            // Renderizar a textura do contador de pontos
            pontuacaoRect.x = 10; // Definir a posição X
            pontuacaoRect.y = 10; // Definir a posição Y
            SDL_QueryTexture(texture, NULL, NULL, &pontuacaoRect.w, &pontuacaoRect.h);
            SDL_RenderCopy(renderizador, texture, NULL, &pontuacaoRect);
            SDL_DestroyTexture(texture);

            SDL_RenderPresent(renderizador);

            SDL_Delay(100);
        }
        else if (estadoJogo == TELA_INICIAL) {
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderClear(renderizador);

            // Exibir mensagem de boas-vindas
            char mensagemInicial[50] = "Pressione qualquer tecla para comecar";
            SDL_Surface* surfaceInicial = TTF_RenderText_Solid(font, mensagemInicial, corBranca);
            if (!surfaceInicial) {
                printf("Erro ao criar superficie de texto: %s\n", TTF_GetError());
                return 1;
            }
            SDL_Texture* textureInicial = SDL_CreateTextureFromSurface(renderizador, surfaceInicial);
            if (!textureInicial) {
                printf("Erro ao criar textura de texto: %s\n", SDL_GetError());
                SDL_FreeSurface(surfaceInicial);
                return 1;
            }
            SDL_FreeSurface(surfaceInicial);

            // Renderizar a mensagem de boas-vindas
            SDL_Rect mensagemInicialRect = { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 25, 300, 50 };
            SDL_QueryTexture(textureInicial, NULL, NULL, &mensagemInicialRect.w, &mensagemInicialRect.h);
            SDL_RenderCopy(renderizador, textureInicial, NULL, &mensagemInicialRect);
            SDL_DestroyTexture(textureInicial);

            SDL_RenderPresent(renderizador);
        }
        else if (estadoJogo == TELA_GAMEOVER) {
            // Limpar a tela com cor preta
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            SDL_RenderClear(renderizador);

            // Exibir pontuação do jogador
            char mensagemPontuacao[50];
            sprintf_s(mensagemPontuacao, 50, "Pontuacao: %d", pontos);
            SDL_Surface* surfacePontuacao = TTF_RenderText_Solid(font, mensagemPontuacao, corBranca);
            if (!surfacePontuacao) {
                printf("Erro ao criar superfície de texto: %s\n", TTF_GetError());
                return 1;
            }
            SDL_Texture* texturePontuacao = SDL_CreateTextureFromSurface(renderizador, surfacePontuacao);
            if (!texturePontuacao) {
                printf("Erro ao criar textura de texto: %s\n", SDL_GetError());
                SDL_FreeSurface(surfacePontuacao);
                return 1;
            }
            SDL_FreeSurface(surfacePontuacao);

            // Renderizar a pontuação do jogador
            SDL_QueryTexture(texturePontuacao, NULL, NULL, &pontuacaoRect.w, &pontuacaoRect.h);
            SDL_RenderCopy(renderizador, texturePontuacao, NULL, &pontuacaoRect);
            SDL_DestroyTexture(texturePontuacao);

            // Exibir mensagem para tentar novamente
            char mensagemGameOver[50] = "Pressione R para tentar novamente";
            SDL_Surface* surfaceGameOver = TTF_RenderText_Solid(font, mensagemGameOver, corBranca);
            if (!surfaceGameOver) {
                printf("Erro ao criar superficie de texto: %s\n", TTF_GetError());
                return 1;
            }
            SDL_Texture* textureGameOver = SDL_CreateTextureFromSurface(renderizador, surfaceGameOver);
            if (!textureGameOver) {
                printf("Erro ao criar textura de texto: %s\n", SDL_GetError());
                SDL_FreeSurface(surfaceGameOver);
                return 1;
            }
            SDL_FreeSurface(surfaceGameOver);

            // Renderizar a mensagem de game over
            SDL_Rect gameOverRect = { SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 50, 400, 50 };
            SDL_QueryTexture(textureGameOver, NULL, NULL, &gameOverRect.w, &gameOverRect.h);
            SDL_RenderCopy(renderizador, textureGameOver, NULL, &gameOverRect);
            SDL_DestroyTexture(textureGameOver);

            SDL_RenderPresent(renderizador);
        }
    }

    TTF_CloseFont(font); // Fechar a fonte
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
