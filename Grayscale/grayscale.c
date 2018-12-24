#include <stdio.h>
#include <stdint.h>

uint32_t xorshift32(unsigned int seed)
{
    seed^= seed<<13;
    seed^= seed>>17;
    seed^= seed<<5;
    return seed;

}

void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie)
{
    FILE *fin, *fout;
    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char pRGB[3], header[54], aux;

    printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

    fin = fopen(nume_fisier_sursa, "rb");
    if(fin == NULL)
    {
        printf("nu am gasit imaginea sursa din care citesc");
        return;
    }

    fout = fopen(nume_fisier_destinatie, "wb+");

    fseek(fin, 2, SEEK_SET);
    fread(&dim_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

    fseek(fin, 18, SEEK_SET);
    fread(&latime_img, sizeof(unsigned int), 1, fin);
    fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

    //copiaza octet cu octet imaginea initiala in cea noua
    fseek(fin,0,SEEK_SET);

    unsigned char c;
    while(fread(&c,1,1,fin)==1)
    {
        fwrite(&c,1,1,fout);
        fflush(fout);
    }
    fclose(fin);

    //calculam padding-ul pentru o linie
    int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    printf("padding = %d \n\n",padding);
    printf("Procesing");
    fseek(fout, 54, SEEK_SET);
    int i,j;

    for(i = 0; i < inaltime_img; i++)
    {
        if(i%100==0)
            printf(".");
        for(j = 0; j < latime_img; j++)
        {
            //citesc culorile pixelului
            fread(pRGB, 3, 1, fout);
            //fac conversia in pixel gri
            pRGB[0] = xorshift32(pRGB[0])%256;
            pRGB[1] = xorshift32(pRGB[1])%256;
            pRGB[2] = xorshift32(pRGB[2])%256;
            fseek(fout, -3, SEEK_CUR);
            fwrite(pRGB, 3, 1, fout);
            fflush(fout);
        }
        fseek(fout,padding,SEEK_CUR);
    }

    fclose(fout);
    printf("\nConversie in grayscale realizata cu succes\n\n");
}


///Generam numere PseudoRandom cu XORSHIFT


void liniarizare(char* nume_fisier_sursa)
{

    FILE *fin;
    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char pRGB[3], header[54], aux, lin[1000000];

    unsigned char *imageData;

    printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

    fin = fopen(nume_fisier_sursa, "rb");
    if(fin == NULL)
    {
        printf("nu am gasit imaginea sursa din care citesc");
        return;
    }

    fseek(fin, 2, SEEK_SET);
    fread(&dim_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

    fseek(fin, 18, SEEK_SET);
    fread(&latime_img, sizeof(unsigned int), 1, fin);
    fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

    //copiaza octet cu octet imaginea initiala in cea noua
    fseek(fin,0,SEEK_SET);
    fread(header, sizeof(unsigned char), 54, fin);
    imageData = (unsigned char*)malloc (latime_img*inaltime_img * sizeof(unsigned char));
    fread( imageData, sizeof(unsigned char), latime_img*inaltime_img, fin);


    fseek(fin,0,SEEK_SET);

    int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    printf("padding = %d \n\n",padding);
    printf("Procesing");

    int i,j,k=0;

    printf("\nLinizarizare realizata cu succes\n\n");

    for(int p=latime_img*inaltime_img; p>=latime_img*inaltime_img-100; p--)
    {
        printf("%d ",imageData[p]);
    }

}

int main()
{
    char nume_img_sursa[] = "source_image.bmp";
    char nume_img_grayscale[] = "result_grayscale.bmp";
    char nume_img_ecrypter[] = "result_ecrypted.bmp";
    int v[1000];
    int *p;
    p = v;
    grayscale_image(nume_img_sursa, nume_img_grayscale);
    liniarizare(nume_img_sursa);

    return 0;
}
