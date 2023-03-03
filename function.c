#include "function.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int window_width = 0;
static int window_height = 0;
static char* name = "SDL app";

void out(){
    printf("Vous dessinez en dehors de la fenetre\n");
}

void checkPos(int x, int y){
    /** @brief verifie que x et y sont bien dans l'ecran initialisé
     *  @param x position à verifier
     *  @param y position à verifier
     */
    if (x < 0) {
        printf("ATTENTION position x < 0 (x = %d)\n", x);
        out();
    }
    if (x > window_width) {
        printf("ATTENTION position x > largeur de la fenetre (x = %d > %d)\n", x, window_width);
        out();
    }
    if (y < 0) {
        printf("ATTENTION position y < 0 (y = %d)\n", x);
        printf("ATTENTION position y > hauteur de la fenetre (y = %d > %d)\n", y, window_height);
        out();
    }
    if (y > window_height) {
        printf("ATTENTION position y > hauteur de la fenetre (y = %d > %d)\n", y, window_height);
        out();
    }
}

void init(int windowWidth, int windowHeight) {
    /** @brief initialise la fenêtre window et le renderer (moteur de rendu) renderer
     *  @param windowWidth la largeur de la fenêtre
     *  @param windowHeight la hauteur de la fenêtre
     */
    window_width = windowWidth;
    window_height = windowHeight;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("ERREUR : Init SDL > %s\nParametres passes %d , %d\n",SDL_GetError(), windowWidth, windowHeight);
        freeAndTerminate();
    }
    if (SDL_CreateWindowAndRenderer(windowWidth, windowHeight, 0, &window, &renderer)) {
        SDL_Log("ERREUR : Init window and renderer > %s\nParametres passes %d , %d\n",SDL_GetError(), windowWidth, windowHeight);
        freeAndTerminate();
    }
    SDL_SetWindowTitle(window, "Brick Breaker");
}

void freeWindow() {
    /** @brief libère la mémoire allouée à la fenêtre
     */
    if (window) SDL_DestroyWindow(window);
}

void freeRenderer() {
    /** @brief libère la mémoire allouée au renderer
     */
    if (renderer) SDL_DestroyRenderer(renderer);
}

void freeTexture(SDL_Texture *texture) {
    /** @brief libère la mémoire allouée à la texture (utilisée pour dans la fonction sprite() pour afficher une image)
     *  @param texture la texture utilisée pour afficher l'image
     */
    if (texture) SDL_DestroyTexture(texture);
}

void freeAndTerminate() {
    /** @brief libère la mémoire allouée au programme(renderer et fenêtre) puis quitte la SDL et le programme
     */
    freeRenderer();
    freeWindow();
    SDL_Quit();
    exit(EXIT_SUCCESS);
}


void actualize() {
    /** @brief actualise le renderer (i.e affiche ce qu'on a dessiné / fait dessus)
     */
    SDL_RenderPresent(renderer);
}

void changeColor(int r, int g, int b) {
    /** @brief change la couleur avec laquelle on dessine sur le renderer en celle du code RGB
     *  @param r Rouge dans le code RGB de la couleur choisi, entre 0 et 255
     *  @param g Vert dans le code RGB de la couleur choisi, entre 0 et 255
     *  @param b Bleu dans le code RGB de la couleur choisi, entre 0 et 255
     */
    if (SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE)) {
        SDL_Log("ERREUR : changement de couleur > %s\nParametres passes %d , %d, %d\n",SDL_GetError(), r, g, b);
        freeAndTerminate();
    }
}

void clear() {
    /** @brief réinitialise le fenêtre et reset la couleur
     *         (i.e supprime ce qu'on a dessiné dessus pour avoir une fenêtre noire)
     */
    changeColor(0, 0, 0);
    SDL_RenderClear(renderer);
    changeColor(255, 255, 255);
}

void drawSquare(int posX, int posY, int longueur) {
    /** @brief dessine un carré
     *  @param posX position sur l'axe horizontale du coin supérieur gauche du carré
     *  @param posY position sur l'axe verticale du coin supérieur gauche du carré
     *  @param longueur longeur du coté du carré
     */
    checkPos(posX + 20, posY + 20);
    checkPos(posX + longueur - 20, posY + longueur - 20);
    SDL_Rect rectangle;
    rectangle.x = posX;
    rectangle.y = posY;
    rectangle.w = longueur;
    rectangle.h = longueur;

    if (SDL_RenderFillRect(renderer, &rectangle)) {
        SDL_Log("ERREUR : Impossible de creer le carre > %s\nParametres passes %d , %d, %d\n",SDL_GetError(), posX, posY, longueur);
        freeAndTerminate();
    }
}

void drawRect(int posX, int posY, int longueur, int hauteur) {
    /** @brief dessine un rectangle
     *  @param posX position sur l'axe horizontale du coin supérieur gauche du rectangle
     *  @param posY position sur l'axe verticale du coin supérieur gauche du rectangle
     *  @param longueur longeur du coté du rectangle
     *  @param hauteur hauteur du coté du carré
     */
    checkPos(posX + 20, posY + 20);
    checkPos(posX + longueur - 20, posY + hauteur - 20);

    for(int y = posY; y < posY + hauteur; y++) {
        for(int x = posX; x < posX + longueur; x++) {
            drawPoint(x, y);
        }
    }
}

void drawCircle(int centerX, int centerY, int rayon) {
    /** @brief dessine un cercle de centre (x,y) et de rayon r sur le renderer
     *         (utilise l'équation d'un cercle x^2 + y^2 <= a^2)
     *  @param centerX position sur l'axe horizontal du centre
     *  @param centerY position sur l'axe vertical du centre
     *  @param rayon le rayon du cercle
     */
    checkPos(centerX - rayon + 20, centerY - rayon + 20);
    checkPos(centerX + rayon - 20, centerY + rayon - 20);
    for (int i = centerX - rayon; i <= centerX + rayon; i++) {
        for(int j = centerY - rayon; j <= centerY +rayon; j++) {
            int x = i - centerX;
            int y = j - centerY;
            if (x * x + y * y <= rayon * rayon) {
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }
    }
}

void drawPoint(int x, int  y) {
    /** @brief dessine un point à la position (x,y) i.e change la couleur du pixel
     *  @param x position sur l'axe horizontal du point
     *  @param y position sur l'axe vertical du point
     */
    SDL_RenderDrawPoint(renderer, x, y);
}

void drawLine(int x1, int y1, int x2, int y2) {
    /** @brief trace une ligne entre le point 1 : (x1,y1) et 2 : (x2,y2)
     *  @param x1 position sur l'axe horizontal du point 1
     *  @param y1 position sur l'axe vertical du point 1
     *  @param x2 position sur l'axe horizontal du point 2
     *  @param y2 position sur l'axe vertical du point 2
     */
    if (SDL_RenderDrawLine( renderer, x1, y1, x2, y2)) {
        SDL_Log("ERREUR : drawLine() > %s\nParametres passes %d , %d, %d, %d\n",SDL_GetError(), x1, y1, x2, y2);
        freeAndTerminate();
    }
}

void drawLinePoints(Point point1, Point point2) {
    /** @brief même fonction que drawLine() mais avec des structure (voir struct Point)
     *  @param point1 Point d'origine de la ligne
     *  @param point2 Point d'arrivée de la ligne
     */
    if (SDL_RenderDrawLine( renderer, point1.x, point1.y, point2.x, point2.y)) {
        SDL_Log("ERREUR : drawLinePoints() > %s\nParametres passes (%d,%d), (%d,%d)\n",SDL_GetError(), point1.x, point1.y, point2.x, point2.y);
        freeAndTerminate();
    }
}

void sprite(int posX, int posY, char *imgBMPSrc) {
    /** @brief affiche un image .bmp sur le renderer
     *  @param posX position sur l'axe horizontale du coin supérieur gauche de l'image
     *  @param posY position sur l'axe verticale du coin supérieur gauche de l'image
     *  @param imgBMPSrc le chemin vers l'image que l'on veut afficher
     */
    checkPos(posX, posY);
    SDL_Texture *textureImg = NULL;
    SDL_Surface *surfaceImg = NULL;
    if (!(surfaceImg = SDL_LoadBMP(imgBMPSrc))) {
        SDL_Log("ERREUR : chargement img > %s\nParametres passes %d , %d, %s\n",SDL_GetError(), posX, posY, imgBMPSrc);
        freeAndTerminate();
    }
    textureImg = SDL_CreateTextureFromSurface(renderer, surfaceImg);
    SDL_FreeSurface(surfaceImg);
    if (textureImg == NULL) {
        SDL_Log("ERREUR : chargement texture > %s\nParametres passes %d , %d, %s\n",SDL_GetError(), posX, posY, imgBMPSrc);
        freeTexture(textureImg);
        freeAndTerminate();
    }

    SDL_Rect rectangle;
    if (SDL_QueryTexture(textureImg, NULL, NULL, &rectangle.w, &rectangle.h)) {
        SDL_Log("ERREUR : image : query texture > %s\nParametres passes %d , %d, %s\n",SDL_GetError(), posX, posY, imgBMPSrc);
        freeTexture(textureImg);
        freeAndTerminate();
    }
    rectangle.x = posX;
    rectangle.y = posY;
    if (SDL_RenderCopy(renderer, textureImg, NULL, &rectangle) != 0) {
        SDL_Log("ERREUR: image : RenderCopy > %s\nParametres passes %d , %d, %s\n",SDL_GetError(), posX, posY, imgBMPSrc);
        freeTexture(textureImg);
        freeAndTerminate();
    }
    freeTexture(textureImg);
}

void lastKeyPressed(SDL_Event *event) {
    /** @brief affiche dans le terminal le caractère associé à la dernière touche appuyée
     *         ne fonctionne que si le type de l'event est SDL_KEYDOWN
     *         la gestion d'évènement se fait avec SDL_PollEvent(&event)
     *  @param event l'evenement qui s'est produit
     */
    if (event->type == SDL_KEYDOWN) {
        printf("Last Key Pressed: %c\n", event->key.keysym.sym);
    }
}