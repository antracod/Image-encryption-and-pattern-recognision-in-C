#include <stdio.h>
#include <stdint.h>

unsigned long int xorshift32(unsigned long int seed)
{
    seed^= seed<<13;
    seed^= seed>>17;
    seed^= seed<<5;
    return seed;
}

unsigned char* ReadBMP(char* nume_fisier_intrare)
{
    FILE* fin = fopen(nume_fisier_intrare, "rb");

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, fin);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width*3 + 3) & (~3);
    unsigned char* data = malloc(sizeof(unsigned char)*row_padded);
    unsigned char* data2 = malloc(sizeof(unsigned char)*row_padded*height+54);

    for(int i= 0 ; i < 54 ; i++)
    {
        data2[i] = info[i];
    }

    int k=54;

    for(int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, fin);
        for(int j = 0; j < width*3; j += 3)
        {
            data2[k++]=data[j];
            data2[k++]=data[j+1];
            data2[k++]=data[j+2];
        }
    }

    fclose(fin);
    return data2;
}

void WriteBMP(char *nume_fisier_iesire,unsigned char *a)
{
    FILE *fout;
    fout = fopen(nume_fisier_iesire,"wb+");

    int width = *(int*)&a[18];
    int height = *(int*)&a[22];

    int psize = width*height*3+54;

    for(int i=0; i<psize; i++)
    {
        fwrite(&a[i], 1, 1, fout);
        fflush(fout);
    }
}

unsigned char *getheader(char *nume_img_sursa)
{
    FILE* f = fopen(nume_img_sursa, "rb");
    unsigned char *hdr = malloc(54*sizeof(unsigned char));
    fread(hdr, sizeof(unsigned char), 54, f);
    fseek(f,0,SEEK_SET);
    return hdr;
}

unsigned char *switchpixels(unsigned char *a,int i,int j)
{
    unsigned char *p = a;
    unsigned char tmp1,tmp2,tmp3;
    tmp1 = p[i*3+54];
    tmp2 = p[i*3+55];
    tmp3 = p[i*3+56];
    p[i*3+54] = p[j*3+54];
    p[i*3+55] = p[j*3+55];
    p[i*3+56] = p[j*3+56];
    p[j*3+54] = tmp1;
    p[j*3+55] = tmp2;
    p[j*3+56] = tmp3;
    return p;
}

int main()
{
    char nume_img_sursa[] = "peppers.bmp";
    char nume_img_criptata[] = "peppers_criptata.bmp";
    char nume_img_ecrypter[] = "peppers_ecrypted.bmp";

    unsigned char *info;
    info = getheader(nume_img_sursa);
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    unsigned char *a;
    a =  malloc(sizeof(unsigned char)*width*height*3+55);

    a = ReadBMP(nume_img_sursa);

    unsigned long int *r;
    int area = width*height;
    r = malloc(sizeof(unsigned long)*2*area);
    r[0]=123456789;
    for(int i=1;i<area*2;i++)
    {
        r[i]=xorshift32(r[i-1]);
    }

     int i, j, tmp;
      unsigned long int *rp;
     rp = malloc(sizeof(unsigned long)*2*area);

     for(int i=0;i<=area;i++)
     {
         rp[i]=i;
     }

     for (i = area - 1; i > 0; i--) {
         j = r[area-1-i] % (i + 1);
         a = switchpixels(a,i,j);
     }

    WriteBMP(nume_img_criptata,a);

    return 0;
}
