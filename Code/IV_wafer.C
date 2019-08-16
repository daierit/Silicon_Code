#include <iostream>
#include <fstream>
#include <string.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TApplication.h>
#include <map>
using namespace std;

void IV_wafer(){
  const double VStart=0;
  double VStop[2]={50,400};
  const double VStep=10;
  const double RangeDown=0;
  const double RangeUp=1000;
  char Filename[200];
  char Title[200];
  vector<string> Celltype{"014","058"};
  char level[20]="nA";
  vector<vector<int>> Wafer;
  vector<int> Wafer_g;
  vector<int> Color{1,2,3,4,6,7,8,9,11,12,28,30,36,38,40,42,46};
  vector<vector<int>> Style{{20,21,22,23},{24,25,26,32},{39,41,43,45},{37,40,42,44}};
  vector<string> Date{}; 
  int argc, ai, ad, readN;
  bool as=false;
  char *p; 
  char const *delim=",";
  argc=gApplication->Argc();
  for(ai=0;ai<argc;++ai){
    printf("a[%d]=%s\n", ai, gApplication->Argv(ai));
    if(as){
      p = strtok(gApplication->Argv(ai),delim);
      Wafer_g.clear();
      while(p){
        if(ai>ad){
          readN = atoi(p);
          Wafer_g.push_back(readN);
        }
        else
          Date.push_back(p);
        p = strtok(NULL,delim);
      }
      if(ai>ad)
        Wafer.push_back(Wafer_g);
    }
    if(!strcmp(gApplication->Argv(ai),"@")){
      as=true;
      ad=ai+1;
    }
  }
  //Read data
  double Vr, Ir;
  map< array<int,4>,vector<double>> V;
  map< array<int,4>,vector<double>> I;
  array<int,4> key;
  vector<double> VV;
  vector<double> II;
  for(int type=0;type<Celltype.size();++type){
    for(int group=0;group<Wafer.size();++group){
      for(int wafer=0;wafer<Wafer[group].size();++wafer){
        for(int date=0;date<Date.size();++date){
          sprintf(Filename,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/KEI_2410/HGcal_6inch_%02d/%s/IV.txt",Date[date].c_str(), Wafer[group][wafer], Celltype[type].c_str());
          cout << Filename << endl;
          fstream r1;
          r1.open(Filename,ios::in);
          VV.clear();
          II.clear();
          if(r1){
            key={type,group,wafer,date};
            while(r1>>Vr>>Ir){
              VV.push_back(Vr);
              II.push_back(Ir);
            }
            V[key]=VV;
            I[key]=II;
          }
          else
            cout << "can't find file:" << Filename <<  endl;
          r1.close();
        }//date
      }//wafer
    }//group
  }// type
  TCanvas *c1 = new TCanvas("c1","IV",1024,768);
  TCanvas *c2 = new TCanvas("c2","Log IV",1024,768);
  gStyle-> SetOptStat(0);
  c1-> cd();
  c2-> SetLogy();

  for(int type=0;type<Celltype.size();++type){
      TLegend *leg = new TLegend(0.8,0.5,1,1,NULL,"brNDC");
      leg->SetBorderSize(0);
      leg->SetTextFont(62);
      leg->SetLineColor(1);
      leg->SetLineStyle(1);
      leg->SetLineWidth(1);
      leg->SetFillColor(0);
      leg->SetFillStyle(1001);
      TMultiGraph *mg = new TMultiGraph();
    for(int group=0;group<Wafer.size();++group){
      for(int wafer=0;wafer<Wafer[group].size();wafer++){
        for(int date=0;date<Date.size();++date){
          key = {type,group,wafer,date};
          TGraph* g1=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
          TGraph* g2=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
          sprintf(Title,"HGCal Ch%s",Celltype[type].c_str());
          g1-> SetTitle(Title);
          g1-> GetXaxis()-> SetTitle("V");
          g1-> GetYaxis()-> SetTitle(level);
          g1-> GetXaxis()-> SetLimits(VStart+VStep,VStop[type]);
          c1->cd();
          g1-> GetYaxis()-> SetRangeUser(RangeDown,RangeUp);
          g1-> SetMarkerStyle(Style[date][wafer]);
          g1-> SetMarkerSize(1);
          g1-> SetMarkerColor(Color[group]);
          if(group==0 && wafer==0 && date==0){
              g1-> Draw("ap");
              g1-> Draw("p");
          }
          else{
            g1-> Draw("p,same");
          }
          sprintf(Filename,"Wafer%d %s",Wafer[group][wafer],Date[date].c_str());
          leg-> AddEntry(g1,Filename,"p");
          c2-> cd();
          g2-> SetTitle(Title);
          g2-> GetXaxis()-> SetTitle("V");
          g2-> GetYaxis()-> SetTitle(level);
          g2-> GetXaxis()-> SetLimits(VStart+VStep,VStop[type]);
          g2-> GetYaxis()-> SetRangeUser(10,10000);
          g2-> SetMarkerStyle(Style[date][wafer]);
          g2-> SetMarkerSize(1);
          g2-> SetMarkerColor(Color[group]);
          if(group==0 && wafer==0 && date==0){
            g2-> Draw("ap");
            g2-> Draw("p");
          }
          else
            g2-> Draw("p,same");
          //mg-> Add(g1);
        }//end day loop
      }//end month loop
      //mg-> Draw("ap");
      //mg-> Draw("p");
    }//end wafer loop
      c1-> cd();
      leg-> Draw();
      sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2/%s_w%d_%.0lf.png",Celltype[type].c_str(),Wafer[0].front(),RangeUp);
      c1-> Print(Filename);
      c2-> cd();
      leg-> Draw();
      sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2/%s_w%d_log.png",Celltype[type].c_str(),Wafer[0].front());
      c2-> Print(Filename);
  }//end type loop

}
