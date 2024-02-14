#ifndef _IMAGE_
#define _IMAGE_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define TAILLE_MAX 1000

/**
 * Structure pour l'image
 */
struct imageNB
{
    int width;
    int height;
    unsigned char **color;
    int vmax;
};

/**
 * Fonction pour charger une image au format .pgm
 * @param img
 * @param nomImage
 */
void loadPGM(struct imageNB *img, char *nomImage)
{
    FILE *fichier = fopen(nomImage, "rb");

    if (fichier != NULL)
    {
        char chaine[3];
        int verif = fscanf(fichier, "%2s", chaine); // Read only 2 characters to avoid buffer overflow

        printf("%s\n", chaine);

        if (chaine[1] == '5') // Equal to the format 'P5'
        {
            fscanf(fichier, "%d %d", &img->width, &img->height);

            fscanf(fichier, "%d", &img->vmax);

            img->color = malloc(img->height * sizeof(unsigned char *));
            if (img->color == NULL)
            {
                printf("ERROR allocating memory\n");
                fclose(fichier);
                return;
            }

            for (int i = 0; i < img->height; i++)
            {
                img->color[i] = malloc(img->width * sizeof(unsigned char));
                if (img->color[i] == NULL)
                {
                    printf("ERROR allocating memory for row %d\n", i);
                    fclose(fichier);
                    return;
                }
            }

            printf("Height = %d\nWidth = %d, %d\n", img->height, img->width, img->vmax);
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    unsigned char tmp;
                    fread(&tmp, sizeof(unsigned char), 1, fichier);
                    img->color[i][j] = tmp;
                }
            }
            fclose(fichier);
        }
        else
        {
            printf("Unknown format\n");
        }
    }
    else
    {
        printf("--> %s not found \n", nomImage);
    }
}

/**
 * Fonction pour enregistrer une image au format .pgm
 * @param img
 * @param nomImage
 */
void savePGM(struct imageNB *img, char *nomImage)
{
    FILE *fichier = fopen(nomImage, "wb");
    if (fichier != NULL)
    {
        fprintf(fichier, "P5\n");
        fprintf(fichier, "%d %d\n255\n", img->width, img->height);
        for (int i = 0; i < img->height; i++)
        {
            for (int j = 0; j < img->width; j++)
            {
                unsigned char tmp = img->color[i][j];
                fwrite(&tmp, sizeof(unsigned char), 1, fichier);
            }
        }
        fclose(fichier);
    }
    else
    {
        printf("Unable to create file: %s \n", nomImage);
    }
}

/**
 * Fonction pour copié une image
 * @param src
 * @param dest
 */
void copyImage(struct imageNB *src, struct imageNB *dest) {
    dest->width = src->width;
    dest->height = src->height;
    dest->vmax = src->vmax;

    // Allocation de mémoire pour la copie
    dest->color = malloc(dest->height * sizeof(unsigned char *));
    for (int i = 0; i < dest->height; i++) {
        dest->color[i] = malloc(dest->width * sizeof(unsigned char));
        for (int j = 0; j < dest->width; j++) {
            dest->color[i][j] = src->color[i][j];
        }
    }
}

/**
 * Fonction qui ajoute un filtre de sobel à une image
 * @param filtreX
 * @param filtreY
 * @param img
 */
void sobel(int filtreX[3][3], int filtreY[3][3], struct imageNB *img)
{
    int sumX, sumY, SumTotal;

    struct imageNB imgSobel;
    imgSobel.width = img->width;
    imgSobel.height = img->height;
    imgSobel.vmax = img->vmax;
    imgSobel.color = malloc(imgSobel.height * sizeof(unsigned char *));

    for (int ii = 0; ii < imgSobel.height; ii++)
    {
        imgSobel.color[ii] = malloc(imgSobel.width * sizeof(unsigned char));
    }

    for (int y = 1; y < img->height - 1; y++)
    {
        for (int x = 1; x < img->width - 1; x++)
        {
            sumX = sumY = SumTotal = 0;

            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int c = img->color[x + i][y + j];
                    sumX += c * filtreX[i + 1][j + 1];
                    sumY += c * filtreY[i + 1][j + 1];
                }
            }

            SumTotal = abs(sumX) + abs(sumY);
            imgSobel.color[x][y] = (SumTotal > 255) ? 255 : SumTotal;
        }
    }
    savePGM(&imgSobel, "./result/sobel.pgm");

    // Free the allocated memory for imgSobel.color
    for (int i = 0; i < imgSobel.height; i++)
    {
        free(imgSobel.color[i]);
    }
    free(imgSobel.color);
}

/**
 * Fonction qui fait une translation sur une image
 * @param img
 * @param decal
 */
void translation(struct imageNB *img, int decal)
{
    struct imageNB tr;
    tr.width = img->width;
    tr.height = img->height;
    tr.vmax = img->vmax;
    tr.color = malloc(tr.height * sizeof(unsigned char *));

    for (int ii = 0; ii < tr.height; ii++)
    {
        tr.color[ii] = malloc(tr.width * sizeof(unsigned char));
    }

    for (int i = 0; i < img->width; i++)
    {
        for (int j = 0; j < img->height; j++)
        {
            int newColumn = (i + decal) % img->width;
            tr.color[j][newColumn] = img->color[j][i];
        }
    }
    savePGM(&tr, "./result/translation.pgm");

    // Free the allocated memory for tr.color
    for (int i = 0; i < tr.height; i++)
    {
        free(tr.color[i]);
    }
    free(tr.color);
}

/**
 * Fonction qui ajoute du seuillage à une image
 * @param img
 * @param seuil
 */
void seuillage(struct imageNB *img, int seuil)
{
    struct imageNB tr;
    tr.width = img->width;
    tr.height = img->height;
    tr.vmax = img->vmax;
    tr.color = malloc(tr.height * sizeof(unsigned char *));

    for (int ii = 0; ii < tr.height; ii++)
    {
        tr.color[ii] = malloc(tr.width * sizeof(unsigned char));
    }

    for (int i = 0; i < img->width; i++)
    {
        for (int j = 0; j < img->height; j++)
        {
            tr.color[j][i] = (img->color[j][i] > seuil) ? 255 : 0;
        }
    }
    savePGM(&tr, "./result/seuillage.pgm");

    // Free the allocated memory for tr.color
    for (int i = 0; i < tr.height; i++)
    {
        free(tr.color[i]);
    }
    free(tr.color);
}

/**
 * Fonction qui redimensionne une image
 * @param img
 */
void redimensionner(struct imageNB *img)
{
    int amoutScale = 3;
    struct imageNB tr;
    tr.width = img->width * amoutScale;
    tr.height = img->height * amoutScale;
    tr.vmax = img->vmax;
    tr.color = malloc(tr.height * sizeof(unsigned char *));

    for (int ii = 0; ii < tr.height; ii++)
    {
        tr.color[ii] = malloc(tr.width * sizeof(unsigned char));
    }

    for (int i = 0; i < img->width; i++)
    {
        for (int j = 0; j < img->height; j++)
        {
            tr.color[j * 2][i * 2] = img->color[j][i];
            tr.color[j * 2 + 1][i * 2] = tr.color[j * 2][i * 2];
            tr.color[j * 2][i * 2 + 1] = tr.color[j * 2][i * 2];
            tr.color[j * 2 + 1][i * 2 + 1] = tr.color[j * 2][i * 2];
        }
    }

    savePGM(&tr, "./result/redimensionner.pgm");

    // Free the allocated memory for tr.color
    for (int i = 0; i < tr.height; i++)
    {
        free(tr.color[i]);
    }
    free(tr.color);
}

/**
 * Fonction qui réalise un histogramme d'une image
 * @param img
 */
void histogramme(struct imageNB *img)
{
    // Calculer l'histogramme
    int histogram[256] = {0};
    int maxCount = 0;

    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            int intensite = img->color[i][j];
            histogram[intensite]++;
            if (histogram[intensite] > maxCount)
            {
                maxCount = histogram[intensite];
            }
        }
    }

    // Définir la largeur de chaque barre
    int barWidth = 20;
    int histoWidth = 256 * barWidth;

    // Créer une image pour représenter l'histogramme
    struct imageNB histo;
    histo.width = histoWidth;
    histo.height = maxCount;
    histo.vmax = 255;

    // Allouer de la mémoire pour le tableau des couleurs
    histo.color = malloc(histo.height * sizeof(unsigned char *));
    for (int i = 0; i < histo.height; i++)
    {
        histo.color[i] = malloc(histo.width * sizeof(unsigned char));
        // Initialiser à zéro
        for (int j = 0; j < histo.width; j++)
        {
            histo.color[i][j] = 0;
        }
    }

    // Remplir l'image de l'histogramme
    for (int i = 0; i < 256; i++)
    {
        int count = histogram[i];
        for (int j = 0; j < count; j++)
        {
            for (int k = 0; k < barWidth; k++)
            {
                int index = i * barWidth + k;
                histo.color[maxCount - 1 - j][index] = 255; // Barres verticales blanches
            }
        }
    }

    // Sauvegarder l'image représentant l'histogramme
    savePGM(&histo, "./result/histogramme.pgm");

    // Libérer la mémoire allouée pour histo.color
    for (int i = 0; i < histo.height; i++)
    {
        free(histo.color[i]);
    }
    free(histo.color);
}

/**
 * Fonction qui ajoute du contraste à une image
 * @param img
 * @param valeurContraste
 */
void contraste(struct imageNB *img, int valeurContraste) {
    // Appliquer le contraste
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int newValue = img->color[y][x] + valeurContraste;
            img->color[y][x] = (newValue > 255) ? 255 : (newValue < 0) ? 0 : newValue;
        }
    }

    // Sauvegarder l'image
    savePGM(img, "./result/contraste.pgm");
}

/**
 * Fonction qui ajoute de la luminosité à une image
 * @param img
 * @param valeurLuminosite
 */
void luminosite(struct imageNB *img, int valeurLuminosite) {
    // Appliquer la luminosité
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int newValue = img->color[y][x] + valeurLuminosite;
            img->color[y][x] = (newValue > 255) ? 255 : (newValue < 0) ? 0 : newValue;
        }
    }

    // Sauvegarder l'image
    savePGM(img, "./result/luminosite.pgm");
}

/**
 * Fonction qui floute une image
 * @param img
 */
void flouter(struct imageNB *img)
{
    struct imageNB imgBlurred;
    imgBlurred.width = img->width;
    imgBlurred.height = img->height;
    imgBlurred.vmax = img->vmax;
    imgBlurred.color = malloc(imgBlurred.height * sizeof(unsigned char *));

    for (int ii = 0; ii < imgBlurred.height; ii++)
    {
        imgBlurred.color[ii] = malloc(imgBlurred.width * sizeof(unsigned char));
    }

    int kernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

    for (int y = 1; y < img->height - 1; y++)
    {
        for (int x = 1; x < img->width - 1; x++)
        {
            int sum = 0;
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    sum += img->color[x + i][y + j] * kernel[i + 1][j + 1];
                }
            }
            imgBlurred.color[x][y] = sum / 9;
        }
    }
    savePGM(&imgBlurred, "./result/flooter.pgm");

    for (int i = 0; i < imgBlurred.height; i++)
    {
        free(imgBlurred.color[i]);
    }
    free(imgBlurred.color);
}

/**
 * Fonction qui fait faire une rotation à une image
 * @param img
 * @param angle
 * @param clockwise
 */
void pivoter(struct imageNB *img, float angle, bool clockwise)
{
    double radians = angle * M_PI / 180.0;

    int newWidth, newHeight;
    if (clockwise)
    {
        newWidth = img->height;
        newHeight = img->width;
    }
    else
    {
        newWidth = img->width;
        newHeight = img->height;
    }

    struct imageNB rotatedImg;
    rotatedImg.width = newWidth;
    rotatedImg.height = newHeight;
    rotatedImg.vmax = img->vmax;
    rotatedImg.color = malloc(rotatedImg.height * sizeof(unsigned char *));

    for (int i = 0; i < rotatedImg.height; i++)
    {
        rotatedImg.color[i] = malloc(rotatedImg.width * sizeof(unsigned char));
    }

    double centerX = img->width / 2.0;
    double centerY = img->height / 2.0;

    for (int i = 0; i < rotatedImg.width; i++)
    {
        for (int j = 0; j < rotatedImg.height; j++)
        {
            double x = i - centerX;
            double y = j - centerY;

            double newX = x * cos(radians) - y * sin(radians);
            double newY = x * sin(radians) + y * cos(radians);

            int originalX, originalY;

            if (clockwise)
            {
                originalX = (int)(newX + centerY);
                originalY = (int)(centerX - newY);
            }
            else
            {
                originalX = (int)(centerX + newX);
                originalY = (int)(newY + centerY);
            }

            if (originalX >= 0 && originalX < img->width && originalY >= 0 && originalY < img->height)
            {
                rotatedImg.color[j][i] = img->color[originalY][originalX];
            }
            else
            {
                rotatedImg.color[j][i] = 0; // Set to black for pixels outside the original image
            }
        }
    }

    char filename[100];
    snprintf(filename, sizeof(filename), "./result/rotation_%d_degrees_%s.pgm", (int)angle,clockwise == 1 ? "in_clockwise" : "not_in_clockwise");
    printf("%s", filename);
    savePGM(&rotatedImg, filename);

    for (int i = 0; i < rotatedImg.height; i++)
    {
        free(rotatedImg.color[i]);
    }
    free(rotatedImg.color);
}

/**
 * Fonction qui ajoute un effet negatif à une image
 * @param img
 */
void negatif(struct imageNB *img)
{
    if (img == NULL || img->color == NULL)
    {
        printf("Invalid image structure\n");
        return;
    }

    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            img->color[i][j] = img->vmax - img->color[i][j];
        }
    }

    savePGM(img, "./result/negatif.pgm");
}

/**
 * Fonction qui pixélise une image
 * @param img
 * @param taillePixel
 */
void pixeliser(struct imageNB *img, int taillePixel)
{
    if (img == NULL || img->color == NULL || taillePixel < 1)
    {
        printf("Invalid parameters for pixeliser function\n");
        return;
    }

    for (int y = 0; y < img->height; y += taillePixel)
    {
        for (int x = 0; x < img->width; x += taillePixel)
        {
            int somme = 0;
            int count = 0;

            // Calculer la somme des valeurs des pixels dans le bloc
            for (int i = 0; i < taillePixel && y + i < img->height; i++)
            {
                for (int j = 0; j < taillePixel && x + j < img->width; j++)
                {
                    somme += img->color[y + i][x + j];
                    count++;
                }
            }

            // Calculer la valeur moyenne
            unsigned char moyenne = somme / count;

            // Appliquer la valeur moyenne à tous les pixels du bloc
            for (int i = 0; i < taillePixel && y + i < img->height; i++)
            {
                for (int j = 0; j < taillePixel && x + j < img->width; j++)
                {
                    img->color[y + i][x + j] = moyenne;
                }
            }
        }
    }

    savePGM(img, "./result/pixeliser.pgm");
}

/**
 * Fonction qui libère la mémoire
 * @param img
 */
void freeImageMemory(struct imageNB *img)
{
    if (img->color != NULL)
    {
        for (int i = 0; i < img->height; i++)
        {
            free(img->color[i]);
        }
        free(img->color);
        img->color = NULL;
    }
}

int main()
{
    // Create an image structure
    struct imageNB myImage;

    // Load the PGM image
    loadPGM(&myImage, "./input.pgm");

    int choix;
    do
    {
        // Afficher le menu
        printf("\nMenu :\n");
        printf("1. Rotation\n");
        printf("2. Filtre de Sobel\n");
        printf("3. Translation\n");
        printf("4. Seuillage\n");
        printf("5. Redimensionner\n");
        printf("6. Histogramme\n");
        printf("7. Contrast\n");
        printf("8. Luminosité\n");
        printf("9. Flou\n");
        printf("10. Négatif\n");
        printf("11. Pixelise\n");
        printf("0. Quit\n");

        // Demander le choix de l'utilisateur
        printf("Choose an option: ");
        scanf("%d", &choix);

        // Créer une copie de l'image originale
        struct imageNB myImageCopy;
        copyImage(&myImage, &myImageCopy);

        float angleRotation = 0.0;
        int clockwise = 0;

        int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

        int translationAmount = 0;

        int thresholdValue = 0;

        float ajustContrastLevel = 0;

        float ajustLuminosityLevel = 0;

        int taillePixel = 0;

        // Appliquer la transformation correspondante en fonction du choix
        switch (choix)
        {
            case 1:
                // Demande l'ange de rotation
                printf("De combien de degrés souhaitez-vous faire pivoter l'image ? \n> ");
                scanf("%f", &angleRotation);

                // Demande si le pivotage est dans le sens d'une aiguille d'une montre ou non
                printf("Dans quel sens souhaitez-vous faire pivoter dans l'image ? \n - Dans le sens d'une aiguille d'une montre (1) \n - Dans le sens inverse d'une aiguille d'une montre (2) \n> ");
                scanf("%d", &clockwise);

                // Apply pivoting to the copied image
                pivoter(&myImageCopy, angleRotation, clockwise == 1 ? true : false);
                break;
            case 2:
                // Apply a Sobel filter to the copied image
                sobel(sobelX, sobelY, &myImageCopy);
                break;
            case 3:
                // Demande du niveau de translation
                printf("Quel est le montant de translation que vous souhaitez appliquer à l'image ? \n> ");
                scanf("%d", &translationAmount);

                // Translate the copied image
                translation(&myImageCopy, translationAmount);
                break;
            case 4:
                // Demande du niveau de seuillage
                printf("Quel est le niveau de seuillage que vous souhaitez appliquer à l'image ? \n> ");
                scanf("%d", &thresholdValue);

                // Apply thresholding to the copied image
                seuillage(&myImageCopy, thresholdValue);
                break;
            case 5:
                // Resize the copied image
                redimensionner(&myImageCopy);
                break;
            case 6:
                // Generate and save histogram for the copied image
                histogramme(&myImageCopy);
                break;
            case 7:
                // Demande du niveau de contraste
                printf("Quel est le niveau de contraste que vous souhaitez appliquer à l'image ? \n> ");
                scanf("%f", &ajustContrastLevel);

                // Adjust contrast for the copied image
                contraste(&myImageCopy, ajustContrastLevel);
                break;
            case 8:
                // Demande du niveau de contraste
                printf("Quel est le niveau de luminosité que vous souhaitez appliquer à l'image ? \n> ");
                scanf("%f", &ajustLuminosityLevel);

                // Adjust brightness for the copied image
                luminosite(&myImageCopy, ajustLuminosityLevel);
                break;
            case 9:
                // Apply blur to the copied image
                flouter(&myImageCopy);
                break;
            case 10:
                // Apply negative to the copied image
                negatif(&myImageCopy);
                break;
            case 11:
                // Demande du niveau de contraste
                printf("Quelle est la taille de pixel que vous souhaitez appliquer à l'image ? \n> ");
                scanf("%d", &taillePixel);

                // Pixelize the copied image with a pixel size of 10
                pixeliser(&myImageCopy, taillePixel);
                break;
            case 0:
                // Quit the program
                break;
            default:
                printf("Invalid option. Please choose a valid option.\n");
                break;
        }

        // Libérer la mémoire allouée pour l'image copiée
        freeImageMemory(&myImageCopy);
    } while (choix != 0);

    // Libérer la mémoire allouée pour l'image originale
    freeImageMemory(&myImage);

    return 0;
}


#endif