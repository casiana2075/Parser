#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>
#include <winbgim.h>
#include <fstream>
#include <ctime>

#define MAX 30

using namespace std;

ifstream fin("fraza.txt");

const int maxreg=MAX, maxcuv=MAX, maxcarac=MAX, maxlit=MAX;

char fraza[2000];
char *p;
char rezultatParsare[2000];

string articole[MAX];
string pronSubst[MAX];
string adjective[MAX];
string verb[MAX];
string conjunctii[MAX];
string frazaRandom;
int termen, termen1, termen2, termen3, termen4;

struct tip_regula
{
    char stanga[MAX];
    char dreapta1[MAX];
    char dreapta2[MAX];
};

tip_regula regula[50];
unsigned int nrReguli;

tip_regula regulad[50];
unsigned int nrRegulid;


struct multime
{
    unsigned int nrElemente;
    char element[MAX][MAX];
};

multime chart[MAX][MAX];

void afiseazaMultime(multime M)
{
    cout<<"{";
    if (M.nrElemente>0)
    {
        for (unsigned int i=1; i<=M.nrElemente-1; i++)
            cout<<M.element[i]<<",";
        cout<<M.element[M.nrElemente];
    }
    cout<<"}";
}

char Propoz[MAX][MAX]; // Propoz[1] -> primul cuv

void adaugaCuvant(char c[MAX], multime & m)
{
    m.nrElemente++;
    strcpy(m.element[m.nrElemente],c);
}

bool EsteIn(char c[MAX], multime mul)
{
    for (unsigned int i=1; i<=mul.nrElemente; i++)
        if (!strcmp(c,mul.element[i]))
            return true;
    return false;
}

void Star(multime X, multime Y, multime& S) // formeaza multimi pe baza regulilor
{
    S.nrElemente=0;
    unsigned int i,j,k;
    for (i=1; i<=X.nrElemente; i++)
    {
        for (j=1; j<=Y.nrElemente; j++)
        {
            cout<<"Caut: "<<X.element[i]<<" si "<<Y.element[j]<<"\n";
            for (k=1; k<=nrReguli; k++)
                if (!(strcmp(regula[k].dreapta1,X.element[i])) && !(strcmp(regula[k].dreapta2,Y.element[j])) && strcmp(regula[k].dreapta2,"."))
                {
                    cout<<"... am gasit in regula "<<regula[k].stanga<<"->"<<regula[k].dreapta1<<" "<<regula[k].dreapta2<<endl;
                    adaugaCuvant(regula[k].stanga,S);
                }
        }
    }
    cout<<"Am format multimea Star:\n";
    afiseazaMultime(S);
    cout<<endl;
}


void Closure(multime S, multime& C) // analizeaza o regula si adauga in C un tip daca aceasta este singulara
{
    unsigned int i;
    bool gata;
    C=S;
    do
    {
        gata=true;
        for (i=1; i<=nrReguli; i++)
        {
            cout<<"Analizez regula: "<<regula[i].stanga<<"->"<<regula[i].dreapta1<<" "<<regula[i].dreapta2<<endl;
            if (EsteIn(regula[i].dreapta1,C) && (!strcmp(regula[i].dreapta2,".")))
                if (!EsteIn(regula[i].stanga,C))
                {
                    adaugaCuvant(regula[i].stanga,C);
                    gata=false;
                }
        }
    }
    while (!gata);
}

void Lookup(int k, multime & L)  // tipul cuvantului k
{
    unsigned int i;
    L.nrElemente=0;
    for (i=1; i<=nrRegulid; i++)
    {
        if (!strcmp(regulad[i].dreapta1,Propoz[k]))
        {
            adaugaCuvant(regulad[i].stanga,L);
        }
    }
}

int numarcuv;

void Parsare(char fraza[2000],int &numarcuv)
{
    multime L, C, S;
    numarcuv=0;
    char *p;
    p=strtok(fraza, " ");
    while(p)
    {
        numarcuv++;
        strcpy(Propoz[numarcuv],p);
//      cout<<Propoz[numarcuv]<<endl;
        p=strtok(NULL, " ");
    }
    for (int k=1; k<=numarcuv; k++)
        cout<<"cuvantul nr. "<<k<<" este: "<<Propoz[k]<<endl;
    for(int k=1; k<=numarcuv; k++)
    {
        cout<<"La cuvantul "<<k<<": "<<Propoz[k]<<"\n";
        Lookup(k,L);
        cout<<"Lookup=";
        afiseazaMultime(L);
        cout<<"Closure:\n";
        Closure(L, C);
        chart[k-1][k]=C;
        afiseazaMultime(chart[k-1][k]);
        for(int i=k-2; i>=0; i--)
        {
            chart[i][k].nrElemente=0;
            for(int j=k-1; j>=i+1; j--)
            {
                Star(chart[i][j], chart[j][k], S);
                for(unsigned int q=1; q<=S.nrElemente; q++)
                {
                    if(!EsteIn(S.element[q], chart[i][k]))
                        adaugaCuvant(S.element[q],chart[i][k]);
                }
            }
            Closure(chart[i][k], C);
            chart[i][k]=C;
            cout<<"aici:\n";
            afiseazaMultime(C);

        }
    }

    if(EsteIn("S", chart[0][numarcuv]))
    {
        cout<<"Fraza este acceptata."<<endl;
        strcpy(rezultatParsare, "Fraza este acceptata.");
    }
    else
    {
        cout<<"Fraza nu este acceptata."<<endl;
        strcpy(rezultatParsare, "Fraza nu este acceptata.");
    }
}

void citesteGramatica()
{
    FILE* reguli;
    char rand[maxreg];
    if (!(reguli = fopen("gram.txt", "r")))
    {
        perror("Eroare la deschidere, cu mesajul de eroare:");
        exit(1);
    }
    int k=0;
    while(fgets(rand,maxreg,reguli))
    {
        rand[strlen(rand)-1]=0;
        //cout<<rand<<endl;
        k++;
        char *p;
        p=strtok(rand,"->");
        strcpy(regula[k].stanga, p);
        //cout<<"stanga: "<<regula[k].stanga<<" ";
        p=strtok(NULL, ">");
        //cout<<"p="<<p<<endl;
        p=strtok(p," ");
        strcpy(regula[k].dreapta1, p);
        //cout<<"dreapta1: "<<regula[k].dreapta1<<" ";
        p=strtok(NULL," ");
        strcpy(regula[k].dreapta2, p);
        //cout<<"dreapta2: "<<regula[k].dreapta2<<" \n";
    }
    nrReguli=k;
    fclose(reguli);
}

void citesteVocabular()
{
    FILE* reguli;
    char rand[maxreg];
    if (!(reguli = fopen("lex.txt", "r")))
    {
        perror("Eroare la deschidere, cu mesajul de eroare:");
        exit(1);
    }
    int k=0;
    while(fgets(rand,maxreg,reguli))
    {
        rand[strlen(rand)-1]=0;
        //cout<<rand<<endl;
        k++;
        char *p;
        p=strtok(rand,"->");
        strcpy(regulad[k].stanga, p);
        //cout<<"stanga: "<<regulad[k].stanga<<" ";
        p=strtok(NULL, ">");
        //cout<<"p="<<p<<endl;
        p=strtok(p," ");
        strcpy(regulad[k].dreapta1, p);
        //cout<<"dreapta1: "<<regulad[k].dreapta1<<"\n";
        strcpy(regulad[k].dreapta2, ".");
    }
    nrRegulid=k;
    fclose(reguli);
}

//////////////////////random//////////////////////

int cntArt=0,cntPronSubst=0,cntVrb=0,cntAdj=0,cntCnj=0;

void sortare()
{
    for(unsigned int i=0; i<=nrRegulid; i++)
    {
        if(strcmp(regulad[i].stanga,"Det")==0)
        {
            articole[cntArt]+=regulad[i].dreapta1;
            articole[cntArt]+=" ";
            cntArt++;
        }
        if(strcmp(regulad[i].stanga,"Pron")==0)
        {
            pronSubst[cntPronSubst]+=regulad[i].dreapta1;
            pronSubst[cntPronSubst]+=" ";
            cntPronSubst++;
        }
        if(strcmp(regulad[i].stanga,"N")==0)
        {
            pronSubst[cntPronSubst]+=regulad[i].dreapta1;
            pronSubst[cntPronSubst]+=" ";
            cntPronSubst++;
        }
        if(strcmp(regulad[i].stanga,"V")==0)
        {
            verb[cntVrb]+=regulad[i].dreapta1;
            verb[cntVrb]+=" ";
            cntVrb++;
        }
        if(strcmp(regulad[i].stanga,"A")==0)
        {
            adjective[cntAdj]+=regulad[i].dreapta1;
            adjective[cntAdj]+=" ";
            cntAdj++;
        }
        if(strcmp(regulad[i].stanga,"C")==0)
        {
            conjunctii[cntCnj]+=regulad[i].dreapta1;
            conjunctii[cntCnj]+=" ";
            cntCnj++;
        }
    }
}

void generareRandom( )
{
    srand(time(0));
    termen = rand() % cntArt;
    termen1 = rand() % cntPronSubst;
    termen2 = rand() % cntVrb;
    termen3 = rand() % cntArt;
    termen4 = rand() % cntPronSubst;


    frazaRandom = articole[termen];
    frazaRandom = frazaRandom + pronSubst[termen1];
    frazaRandom = frazaRandom + verb[termen2];
    frazaRandom = frazaRandom + articole[termen3];
    frazaRandom = frazaRandom + pronSubst[termen4];

}

////////////////////grafica////////////////////

struct punct
{
    int x, y;
};

struct dreptunghi
{
    punct SS, DJ;
};

bool apartine(punct P, dreptunghi D)
{
    return D.SS.x<=P.x  &&  P.x<=D.DJ.x  &&  D.SS.y<=P.y  &&  P.y<=D.DJ.y+35;
}

struct buton
{
    dreptunghi D;
    int culoare;
    char text[20];
};

buton B[15];
int nrButoane=4;

void fullscreen(int & width, int & height)
{
    height=GetSystemMetrics(SM_CYSCREEN);
    width=GetSystemMetrics(SM_CXSCREEN);
}

void deseneazaMeniul()
{
    setcolor(BLACK);
    rectangle(0,200,getmaxx(),getmaxy());
    setcolor(YELLOW);
    setfillstyle(SOLID_FILL,BLACK);
    int i;
    for (i=1; i<=nrButoane; i++)
    {
        B[i].D.SS.x=140*float(i+2.5);
        B[i].D.DJ.x=140*float(i+3.5)-10;
        B[i].D.SS.y=80;
        B[i].D.DJ.y=90;
        switch(i)
        {
        case 1:
            strcpy(B[i].text,"SCRIE FRAZA");
            break;
        case 2:
            strcpy(B[i].text,"VERIFICA");
            break;
        case 3:
            strcpy(B[i].text,"SCHIMBA FRAZA");
            break;
        case 4:
            strcpy(B[i].text,"IESIRE");
            break;
        }
        rectangle(B[i].D.SS.x, B[i].D.SS.y,B[i].D.DJ.x,B[i].D.DJ.y);
        bar(B[i].D.SS.x, B[i].D.SS.y+35, B[i].D.DJ.x, B[i].D.SS.y);
        setbkcolor(BLACK);
        outtextxy(B[i].D.SS.x+((130-8*strlen(B[i].text))/2),B[i].D.SS.y+10,B[i].text);
    }
}

int butonAles()
{
    int i;
    punct p;
    if (ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        p.x=mousex();
        p.y=mousey();
        for (i=1; i<=nrButoane; i++)
            if (apartine(p,B[i].D))
                return i;
    }
    return 0;
}

////////////////////////grafica matrice////////////////////////

void desenareMatrice(int numarcuv, int xmax, int ymax)
{
    float cnt=(float)3/numarcuv;
    float i=0;
    //cout<<cnt;
    while(i<=numarcuv)
    {
        setcolor(WHITE);
        line(0.5*xmax/4+cnt*i*0.5*xmax/2,ymax/2-20,0.5*xmax/4+cnt*i*0.5*xmax/2,ymax-20); //linie
        line(0.5*xmax/4,ymax/2+ymax*i*cnt/6-20,3.5*xmax/4,ymax/2+ymax*i*cnt/6-20); //coloana
        i++;
    }
}

void valoriMatrice(int numarcuv,int xmax, int ymax,int i,float inaltime)
{
    float cnt=(float)3/numarcuv;
    int h;
    for (int j=1; j<=numarcuv; j++)
    {
        h=j-1;
        setcolor(YELLOW);
        settextstyle(10, 0, 1);
        outtextxy(0.5*xmax/4+cnt*h*0.5*xmax/2+10*cnt,ymax/2-20+40*cnt*inaltime, "{");
        float latime=textwidth("{");
        for(unsigned int k=1; k<chart[i][j].nrElemente; k++)
        {
            outtextxy(0.5*xmax/4+cnt*h*0.5*xmax/2+10*cnt+latime,ymax/2-20+40*cnt*inaltime, chart[i][j].element[k]);
            latime+=textwidth(chart[i][j].element[k]);
            outtextxy(0.5*xmax/4+cnt*h*0.5*xmax/2+10*cnt+latime,ymax/2-20+40*cnt*inaltime, ",");
            latime+=textwidth(",");
        }
        outtextxy(0.5*xmax/4+cnt*h*0.5*xmax/2+10*cnt+latime,ymax/2-20+40*cnt*inaltime, chart[i][j].element[chart[i][j].nrElemente]);
        latime+=textwidth(chart[i][j].element[chart[i][j].nrElemente]);
        outtextxy(0.5*xmax/4+cnt*h*0.5*xmax/2+10*cnt+latime,ymax/2-20+40*cnt*inaltime, "}");
    }
    h++;

}

int main()
{

    citesteGramatica();
//  cout<<"\nGramatica:\n\n";
//  for (int i=1;i<=nrReguli;i++)
//      cout<<regula[i].stanga<<"->"<<regula[i].dreapta1<<" "<<regula[i].dreapta2<<endl;
    citesteVocabular();
//  cout<<"\nVocabularul:\n\n";
//  for (int i=1;i<=nrRegulid;i++)
//      cout<<regulad[i].stanga<<"->"<<regulad[i].dreapta1<<" "<<regulad[i].dreapta2<<endl;

    sortare();
    int xmax, ymax;
    fullscreen(xmax, ymax);

    initwindow(xmax, ymax, "Parser - analiza sintactica si lexicala a unei fraze");
    readimagefile("background.jpg", 0, 0, xmax, ymax);
    deseneazaMeniul();

    setcolor(YELLOW);
    settextstyle(0, 0, 3);
    outtextxy(xmax/7,200-10,"Parser - analiza sintactica si lexicala a unei fraze");

    int comanda, butonul_apasat=0, butonul_apasat_anterior=0,ok=1;
    do
    {
        butonul_apasat=butonAles();
        if (butonul_apasat!=0)
        {
            comanda = butonul_apasat;
            cout<<"Comanda "<<comanda<<endl;
            if(butonul_apasat==1 && butonul_apasat_anterior==0)
            {
                setcolor(BLACK);
                rectangle(0.5*xmax/4, 2*ymax/6,3.5*xmax/4,3*ymax/6);
                bar(0.5*xmax/4, 2*ymax/6,3.5*xmax/4,3*ymax/6);
                setcolor(YELLOW);
                settextstyle(8, 0, 1);
                outtextxy(0.7*xmax/4, 2*ymax/6+30, "Fraza este:");
                fin.getline(fraza,2000);
                outtextxy(0.7*xmax/4+130, 2*ymax/6+30, fraza);
                butonul_apasat_anterior=butonul_apasat;
            }
            if(butonul_apasat==2 && ( butonul_apasat_anterior==1 || butonul_apasat_anterior==3 ) )
            {
                Parsare(fraza,numarcuv);
                setcolor(YELLOW);
                outtextxy(0.7*xmax/4, 2*ymax/6+60, rezultatParsare);
                setcolor(BLACK);
                rectangle(0.5*xmax/4, ymax/2-20,3.5*xmax/4,ymax);
                bar(0.5*xmax/4, ymax/2-20,3.5*xmax/4,ymax);
                desenareMatrice(numarcuv,xmax,ymax);
                float inaltime=1.0;
                for(int i=0; i<numarcuv; i++)
                {
                    valoriMatrice(numarcuv,xmax,ymax,i,inaltime);
                    inaltime+=3.72;
                }

                setcolor(YELLOW);
                butonul_apasat_anterior=butonul_apasat;
            }
            if(butonul_apasat==3 && ( butonul_apasat_anterior==1 || butonul_apasat_anterior==2 || butonul_apasat_anterior==3 ))
            {
                setcolor(BLACK);
                rectangle(0.5*xmax/4, 2*ymax/6,3.5*xmax/4,ymax);
                bar(0.5*xmax/4, 2*ymax/6,3.5*xmax/4,ymax);
                setcolor(YELLOW);
                outtextxy(0.7*xmax/4, 2*ymax/6+30, "Fraza este:");
                fin.getline(fraza,2000);
                if(strcmp(fraza,".")==0)
                    ok=0;
                if(ok==0)
                {
                    generareRandom();
                    strcpy(fraza,frazaRandom.c_str());
                }
                outtextxy(0.7*xmax/4+130,2*ymax/6+30, fraza);
                butonul_apasat_anterior=butonul_apasat;
            }
        }
    }
    while (comanda!=4);
    //getch();
    closegraph();
}
