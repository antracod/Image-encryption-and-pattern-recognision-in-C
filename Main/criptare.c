#include <stdio.h>
#include <stdint.h>

unsigned long int xorshift32(unsigned long int seed)
{
    seed^= seed<<13;
    seed^= seed>>17;
    seed^= seed<<5;
    return seed;
}

void ReadBMP(char* nume_fisier_intrare,unsigned char *data)
{
    FILE* fin = fopen(nume_fisier_intrare, "rb");

    int k=0;
    unsigned int dim_img, latime_img, inaltime_img;
    unsigned char pRGB[3],c;

    fseek(fin, 18, SEEK_SET);
    fread(&latime_img, sizeof(unsigned int), 1, fin);
    fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    fseek(fin,0,SEEK_SET);

    while(fread(&c,1,1,fin)==1)
    {
        data[k++]=c;
    }

    fclose(fin);
}

void WriteBMP(char *nume_fisier_iesire,unsigned char *data)
{
    FILE *fout; /// Fisiere
    fout = fopen(nume_fisier_iesire,"wb+");

    int padding,k; /// Declarari
    int latime_img = *(int*)&data[18];
    int inaltime_img = *(int*)&data[22];
    unsigned char pRGB[3];

    if(latime_img % 4 != 0) /// Aflu padding-ul
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    for(k=0; k<54; k++) /// Scriu header-ul
    {
        fwrite(&data[k],1,1,fout);
    }

    for(int i = 0; i < inaltime_img; i++)
    {
        for(int j = 0; j < latime_img; j++) /// Scriu pixel cu pixel
        {
            pRGB[0] = data[k++];
            pRGB[1] = data[k++];
            pRGB[2] = data[k++];
            fwrite(pRGB, 3, 1, fout);
            fflush(fout);
        }
        for(int s=1; s<=padding; s++) /// Scriu padding-ul
        {
            fwrite(&data[k], 1, 1, fout);
            fflush(fout);
            k++;
        }
    }
    fclose(fout);
}

unsigned char *getheader(char *nume_img_sursa)
{
    FILE* f = fopen(nume_img_sursa, "rb");
    unsigned char *hdr = malloc(54*sizeof(unsigned char));
    fread(hdr, sizeof(unsigned char), 54, f);
    fseek(f,0,SEEK_SET);
    return hdr;
}

void reverse_array(unsigned char *data,unsigned char *header)
{

    int latime_img = *(int*)&header[18];
    int inaltime_img = *(int*)&header[22];
    int padding;

    if(latime_img % 4 != 0) /// Aflu padding-ul
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    int linie=(latime_img*3+padding);

    for(int j=0; j<inaltime_img/2; j++)
    {
        for(int i=0; i<linie; i++)
        {
            unsigned char tmp = data[54+j*linie+i];
            data[54+j*linie+i] = data[54+(inaltime_img-j-1)*linie+i];
            data[54+(inaltime_img-j-1)*linie+i] = tmp;
        }
    }
}

void genrnd_array(unsigned long *rnd,int n,unsigned long int seed)
{
    rnd[0]=seed;

    for(int i=1;i<=n;i++)
    {
        rnd[i]=xorshift32(rnd[i-1]);
    }
}

void genperm_array(unsigned long *perm,unsigned long *rng,int n)
{
    unsigned int k,temp;
    for(k=0;k<=n;k++)
    {
        perm[k]=k;
    }
    for(int k=n-1;k>0;k--)
    {
        local_rng = rng[k];

    }
}

int main()
{
    char nume_img_sursa[] = "peppers.bmp";
    char nume_img_criptata[] = "peppers_criptata.bmp";
    char nume_img_ecrypter[] = "peppers_ecrypted.bmp";

    unsigned char *header;
    header = getheader(nume_img_sursa);
    int latime_img = *(int*)&header[18];
    int inaltime_img = *(int*)&header[22];
    int area = latime_img*inaltime_img;

    unsigned char *data;
    unsigned long int *rnd;
    unsigned long int *perm;

    data =  malloc(sizeof(unsigned char)*inaltime_img*latime_img*3+100);
    rnd = malloc(sizeof(unsigned long int)*inaltime_img*latime_img*2+2);
    perm = malloc(sizeof(unsigned long int)*inaltime_img*latime_img*2+2);

    ReadBMP(nume_img_sursa,data);

    reverse_array(data,header);

    genrnd_array(rnd,inaltime_img*latime_img*2,123456789);

    genperm_array(perm,rnd,inaltime_img*latime_img);

    WriteBMP(nume_img_criptata,data);

    return 0;
}
