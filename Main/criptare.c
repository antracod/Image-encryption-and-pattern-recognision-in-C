#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/// Structuri

typedef struct
{
    double correlation;
    int start_i,start_j,stop_i,stop_j,digit;
} digit_template;

/// Functii

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

    for(int i=1; i<=n; i++)
    {
        rnd[i]=xorshift32(rnd[i-1]);
    }
}

void genperm_array(unsigned long *perm,unsigned long *rng,int n)
{
    unsigned long int k,temp;
    for(k=0; k<=n; k++)
    {
        perm[k]=k;
    }
    for(int k=n-1; k>=1; k--)
    {
        unsigned long local_rng = rng[n-k]%(k+1);
        temp = perm[k];
        perm[k] = perm[local_rng];
        perm[local_rng] = temp;
    }
}

void switch_pixels(unsigned char *data,unsigned x,unsigned y,unsigned latime_img,unsigned padding)
{
    x=(x-1)*3+((x-1)/latime_img)*padding+54;
    y=(y-1)*3+((y-1)/latime_img)*padding+54;
    unsigned char temp1 = data[x];
    unsigned char temp2 = data[x+1];
    unsigned char temp3 = data[x+2];
    data[x] = data[y];
    data[x+1] = data[y+1];
    data[x+2] = data[y+2];
    data[y] = temp1;
    data[y+1] = temp2;
    data[y+2] = temp3;
}

void transfer_pixels(unsigned char *data,unsigned char *data2,unsigned x,unsigned y,unsigned latime_img,unsigned padding)
{
    x=(x-1)*3+((x-1)/latime_img)*padding+54;
    y=(y-1)*3+((y-1)/latime_img)*padding+54;

    data2[y] = data[x];
    data2[y+1] = data[x+1];
    data2[y+2] = data[x+2];
}

unsigned char *apply_perm(unsigned char *data,unsigned long int *perm,unsigned int n,unsigned int latime_img)
{
    unsigned char *data2;
    data2 =  malloc(sizeof(unsigned char)*latime_img*latime_img*3+100);
    for(int i=0; i<=54+3*latime_img*latime_img+5; i++)
    {
        unsigned char tmp;
        tmp = data[i];
        data2[i]=tmp;
    }
    for(int i=0; i<=n-1; i++)
    {
        unsigned long int j=perm[i];
        transfer_pixels(data,data2,i+1,j+1,latime_img,latime_img%4);
    }
    return data2;
}

void xor_substitution(unsigned char *data,unsigned long sv,unsigned latime_img,unsigned long *rnd)
{
    unsigned int bit1 = (sv >> (8*0)) & 0xff;
    unsigned int bit2 = (sv >> (8*1)) & 0xff;
    unsigned int bit3 = (sv >> (8*2)) & 0xff;

    data[54]=bit1^data[54];
    data[55]=bit2^data[55];
    data[56]=bit3^data[56];

    bit1 = (rnd[latime_img*latime_img] >> (8*0)) & 0xff;
    bit2 = (rnd[latime_img*latime_img] >> (8*1)) & 0xff;
    bit3 = (rnd[latime_img*latime_img] >> (8*2)) & 0xff;

    data[54]=data[54]^bit1;
    data[55]=data[55]^bit2;
    data[56]=data[56]^bit3;

    for(int i=2; i<=latime_img*latime_img; i++)
    {
        unsigned nr_pixel=(i-1)*3+((i-1)/latime_img)*(latime_img%4)+54;

        unsigned qpadding;
        if((i-1)%latime_img!=0)
            qpadding=0;
        else
            qpadding=latime_img%4;

        data[nr_pixel]=data[nr_pixel-3-qpadding]^data[nr_pixel];
        data[nr_pixel+1]=data[nr_pixel-2-qpadding]^data[nr_pixel+1];
        data[nr_pixel+2]=data[nr_pixel-1-qpadding]^data[nr_pixel+2];


        bit1 = (rnd[latime_img*latime_img+i-1] >> (8*0)) & 0xff;
        bit2 = (rnd[latime_img*latime_img+i-1] >> (8*1)) & 0xff;
        bit3 = (rnd[latime_img*latime_img+i-1] >> (8*2)) & 0xff;

        data[nr_pixel]=data[nr_pixel]^bit1;
        data[nr_pixel+1]=data[nr_pixel+1]^bit2;
        data[nr_pixel+2]=data[nr_pixel+2]^bit3;
    }
}

unsigned char *xor_substitution2(unsigned char *data,unsigned long sv,unsigned latime_img,unsigned long *rnd)
{
    unsigned char *data2;
    data2 =  malloc(sizeof(unsigned char)*latime_img*latime_img*3+100);

    for(int i=0; i<54; i++)
        data2[i]=data[i];

    unsigned int bit1 = (sv >> (8*0)) & 0xff;
    unsigned int bit2 = (sv >> (8*1)) & 0xff;
    unsigned int bit3 = (sv >> (8*2)) & 0xff;

    data2[54]=bit1^data[54];
    data2[55]=bit2^data[55];
    data2[56]=bit3^data[56];

    bit1 = (rnd[latime_img*latime_img] >> (8*0)) & 0xff;
    bit2 = (rnd[latime_img*latime_img] >> (8*1)) & 0xff;
    bit3 = (rnd[latime_img*latime_img] >> (8*2)) & 0xff;

    data2[54]=data2[54]^bit1;
    data2[55]=data2[55]^bit2;
    data2[56]=data2[56]^bit3;

    for(int i=2; i<=latime_img*latime_img; i++)
    {
        unsigned nr_pixel=(i-1)*3+((i-1)/latime_img)*(latime_img%4)+54;
        unsigned qpadding;
        if((i-1)%latime_img!=0)
            qpadding=0;
        else
            qpadding=latime_img%4;

        data2[nr_pixel]=data[nr_pixel-3-qpadding]^data[nr_pixel];
        data2[nr_pixel+1]=data[nr_pixel-2-qpadding]^data[nr_pixel+1];
        data2[nr_pixel+2]=data[nr_pixel-1-qpadding]^data[nr_pixel+2];


        bit1 = (rnd[latime_img*latime_img+i-1] >> (8*0)) & 0xff;
        bit2 = (rnd[latime_img*latime_img+i-1] >> (8*1)) & 0xff;
        bit3 = (rnd[latime_img*latime_img+i-1] >> (8*2)) & 0xff;

        data2[nr_pixel]=data2[nr_pixel]^bit1;
        data2[nr_pixel+1]=data2[nr_pixel+1]^bit2;
        data2[nr_pixel+2]=data2[nr_pixel+2]^bit3;
    }
    return data2;
}

void geninvperm_array(unsigned long *perm,unsigned long *invperm,int n)
{


    for(int i=0; i<n; i++)
    {
        invperm[perm[i]]=i;
    }
}

void encrypt_image(char *nume_img_sursa,char *nume_img_criptata)
{
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

    data = apply_perm(data,perm,inaltime_img*latime_img,latime_img);

    xor_substitution(data,987654321,latime_img,rnd);

    reverse_array(data,header);

    WriteBMP(nume_img_criptata,data);

}

void decrypt_image(char *nume_img_sursa,char *nume_img_decrypted)
{
    unsigned char *header;
    header = getheader(nume_img_sursa);
    int latime_img = *(int*)&header[18];
    int inaltime_img = *(int*)&header[22];
    int area = latime_img*inaltime_img;

    unsigned char *data;
    unsigned long int *rnd;
    unsigned long int *perm;
    unsigned long int *perm_inv;

    data =  malloc(sizeof(unsigned char)*inaltime_img*latime_img*3+100);
    rnd = malloc(sizeof(unsigned long int)*inaltime_img*latime_img*2+2);
    perm = malloc(sizeof(unsigned long int)*inaltime_img*latime_img*2+2);
    perm_inv = malloc(sizeof(unsigned long int)*inaltime_img*latime_img*2+2);
    ReadBMP(nume_img_sursa,data);

    reverse_array(data,header);

    genrnd_array(rnd,inaltime_img*latime_img*2,123456789);

    genperm_array(perm,rnd,inaltime_img*latime_img);

    geninvperm_array(perm,perm_inv,inaltime_img*latime_img);

    data = xor_substitution2(data,987654321,latime_img,rnd);

    data = apply_perm(data,perm_inv,inaltime_img*latime_img,latime_img);

    reverse_array(data,header);

    WriteBMP(nume_img_decrypted,data);

}

void chi_test(char *nume_img_sursa)
{
    unsigned char *header;
    header = getheader(nume_img_sursa);
    int latime_img = *(int*)&header[18];
    int inaltime_img = *(int*)&header[22];

    unsigned char *data;
    data =  malloc(sizeof(unsigned char)*inaltime_img*latime_img*3+100);

    ReadBMP(nume_img_sursa,data);

    int k=54;
    double pR=0,pG=0,pB=0;
    int fvR[257],fvG[257],fvB[257];
    double f_sub = (latime_img*inaltime_img)/256;


    for(int i=0; i<=256; i++)
    {
        fvR[i] = fvG[i] = fvB[i] = 0;
    }

    for(int i = 0; i < inaltime_img; i++)
    {
        for(int j = 0; j < latime_img; j++)
        {
            fvR[(int)(data[k++])]++;
            fvG[(int)(data[k++])]++;
            fvB[(int)(data[k++])]++;
        }
        k+=latime_img%4;
    }

    for(int i=0; i<=255; i++)
    {
        pR += ((fvR[i]-f_sub)*(fvR[i]-f_sub))/f_sub;
        pG +=  ((fvG[i]-f_sub)*(fvG[i]-f_sub))/f_sub;
        pB += ((fvB[i]-f_sub)*(fvB[i]-f_sub))/f_sub;
    }
    printf("R: %f G: %f B: %f \n",pB,pG,pR);

}



/// For template matching


unsigned char *grayscale_convert(unsigned char *data)
{
    int latime_img = *(int*)&data[18];
    int inaltime_img = *(int*)&data[22];
    int padding;
    unsigned char pRGB[4];

    if(latime_img % 4 != 0) /// Aflu padding-ul
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    int k=54;

    for(int i = 0; i < inaltime_img; i++)
    {
        for(int j = 0; j < latime_img; j++) /// Scriu pixel cu pixel
        {

            pRGB[0] = data[k++];
            pRGB[1] = data[k++];
            pRGB[2] = data[k++];
            data[k-1] = data[k-2] = data[k-3] = 0.299*pRGB[0] + 0.587*pRGB[1] + 0.114*pRGB[2];
        }
        k+=padding;
    }
    return data;
}


void  template_matching()
{

}

void elim_nonmax()
{

}



int main()
{
    char nume_img_sursa[] = "peppers.bmp";
    char nume_img_criptata[] = "peppers_criptata.bmp";
    char nume_img_decriptata[] = "peppers_decrypted.bmp";
    char nume_img_test[] = "test.bmp";
    char nume_img_test_grayscale[] = "testgray.bmp";
    char nume_img_matched[] = "matched.bmp";

    /// Criptare
    encrypt_image(nume_img_sursa,nume_img_criptata);

    /// Decriptare
    decrypt_image(nume_img_criptata,nume_img_decriptata);

    /// Chi test
    chi_test(nume_img_sursa);
    chi_test(nume_img_criptata);

    /// Pattern Matching
    template_matching();

    /// Eliminare non maxime
    elim_nonmax();

    return 0;
}
