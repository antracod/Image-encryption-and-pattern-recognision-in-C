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
            printf("R: %d ",data2[k-1]);
            data2[k++]=data[j+1];
            printf("R: %d",data2[k-1]);
            data2[k++]=data[j+2];
            printf("R: %d",data2[k-1]);

        }
        printf("\n");
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
     int row_padded = (width*3 + 3) & (~3);
    int psize = height*row_padded+54;

    for(int i=0; i<psize; i++)
    {
        fwrite(&a[i], 1, 1, fout);

      //  fflush(fout);
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
    int area = width*height;
    unsigned char *a;
    a =  malloc(sizeof(unsigned char)*width*height*4+55);

    a = ReadBMP(nume_img_sursa);

    WriteBMP(nume_img_criptata,a);


    switchpixels(a,0,1);

    for(int i=0;i<area/2;i++)
     {
         a = switchpixels(a,i,area-i);
     }


    unsigned long int *r;


    r = malloc(sizeof(unsigned long)*3*area);
    r[0]=123456789;
    for(int i=1;i<=area*3;i++)
    {
        r[i]=xorshift32(r[i-1]);
    }

     int i, j, tmp;
      unsigned long int *rp;


     for (i = area; i > 0; i--) {
         j = r[area-1-i] % (i + 1);
         a = switchpixels(a,i,j);
     }



    unsigned long int sv = 987654321;

     a[54] = sv^a[54]^r[area];
     a[55] = sv^a[55]^r[area];
     a[56] = sv^a[56]^r[area];

     for(i = 57;i<=area*3+56;i=i+3)
     {
         if(i%8000==0)
            printf("%d ",i);
        a[i]=a[i-3]^a[i]^r[area+1+(i-57)/3];
        a[i+1]=a[i-2]^a[i+1]^r[area+(i-57)/3];
        a[i+2]=a[i-1]^a[i+2]^r[area+(i-57)/3];
     }
     for(int i=0;i<area/2;i++)
     {
         a = switchpixels(a,i,area-i);
     }

   // WriteBMP(nume_img_criptata,a);

    return 0;
}
