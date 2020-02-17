#include <iostream>
#include <fstream>
#include <string.h>
#include <TCanvas.h>
#include <TH2Poly.h>
#include <TROOT.h>
#include <TStyle.h>
using namespace std;

int main(int argc, char *argv[]){
  char *Date;
  int Wafer;
  vector<int> Volt_cut;
  for(int i=0; i<argc;++i){
    cout << i << " " << argv[i] << endl;
  }
  if(argc>3){
    Date = argv[1];
    Wafer = atoi(argv[2]);
    for(int i=3;i<argc;++i)
      Volt_cut.push_back(atoi(argv[i]));
  }
  else{
    cout << "Please give date, wafer number and volt(./**** 200106 1 100)" << endl;
    return 0;
  }

  gStyle-> SetOptStat(0);
  TCanvas* c0 = new TCanvas("c0","",1300,900);
  c0-> cd();
  const int Num_Folder=1;
  char Floder[Num_Folder][200]={"191225_NCU/ProbeCard/HGcal_6inch_01"};
  sprintf(Floder[0],"%s_NCU/ProbeCard/HGcal_6inch_%02d",Date,Wafer);
  double dlimit=0;
  double ulimit=3000;

  const Int_t Number = 7;
  Double_t Red[Number]   = { 0., 0.5, 0., 1., 1., 0.5, 0};
  Double_t Green[Number] = { 0.5, 1., 1., 1., 0., 0., 0};
  Double_t Blue[Number]  = { 1., 1., 0., 0., 0., 1, 0};
  Double_t Stops[Number] = { 0., 0.002, 0.01, 0.04, 0.1, 0.2, 1 };    
  Int_t nb= 2048;
  TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,nb);

  /*************load channel map*****************/
  int num_w, num_b, num_x, num_u;
  int ch[133],x[133],u[133];
  string inputStr;
  fstream map;
  map.open("channel_map.txt",ios::in);
  if(map){
    getline(map,inputStr);
    while(map >> num_w >> num_b >> num_x >> num_u){
      ch[num_w - 1] = num_b;
      x[num_w - 1] = num_x;
      u[num_w - 1] = num_u;
      //cout << num_w << "\t" << ch[num_w - 1] << "\t" << x[num_w - 1] << "\t" << u[num_w - 1] << endl;
    }
  }
  else
    cout << "can't load map file" << endl;
  map.close();
  /**********************************************/

  /***********Creat/inistal HexCell**************/ 
  TH2Poly* hex = new TH2Poly();
  hex-> Honeycomb(0,0,0.6,12,15);
  int Bins = hex-> GetNumberOfBins();
  for(int i=0;i<Bins+1;++i){
    hex-> SetBinContent(i,-99);
  }
  /**********************************************/

  fstream file;
  double V,I;
  char path[200], name[200];
  char *p1, *p2;
  for(int ivc=0;ivc<Volt_cut.size();++ivc){
  for(int wafer=0;wafer<Num_Folder;++wafer){
    for(int i=0;i<133;++i){
      sprintf(path,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s/%03d/IV.txt",Floder[wafer],i+1);
      file.open(path,ios::in);
      if(file){
        while(file >> V >> I)
          //cout << V << "\t" << I << endl;
          if(V == Volt_cut[ivc]){
            hex-> SetBinContent(ch[i],I);
            break;
          }
          else
            if(I<200)
              hex-> SetBinContent(ch[i],10);
            else
              hex-> SetBinContent(ch[i],10000);
      }
      else{
        cout << "cna't open " << path << endl;
        hex-> SetBinContent(ch[i],-999);
      }
      file.close();
    }//end channel loop

    strcpy(name, Floder[wafer]);
    p2= strtok(name,"NCU");
    strcpy(name, Floder[wafer]);
    p1= strtok(name,"/");
    p1= strtok(NULL,"/");
    p1= strtok(NULL,"/");
    sprintf(path,"/home/kycheng/Silicon_plot/FullWafer/%s_%d.png",p1,Volt_cut[ivc]);

    hex-> SetTitle(Form("%s @ %dV",p1,Volt_cut[ivc]));
    hex-> Draw("colz,text");
    hex-> SetContour(nb);
    hex-> GetZaxis()-> SetRangeUser(dlimit,ulimit);

    //gSystem-> Exec("mkdir /home/kycheng/Silicon_plot/FullWafer");
    c0-> Print(path);    
  }//end wafer loop
  }
  /**************plot channel map*************/
  for(int i=0;i<133;++i)
    hex-> SetBinContent(ch[i],i+1);
  hex-> SetTitle("channel map");
  hex-> GetZaxis()-> SetRangeUser(0,133);
  hex-> Draw("colz,text");
  c0-> Print("/home/kycheng/Silicon_plot/FullWafer/channel_map.png");
  /*******************************************/

  return 0;
}
