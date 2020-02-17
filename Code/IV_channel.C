/**************************************/
//plot multi-group wafers IV data in one or some date
//root @ [date1,data2,...] [wafer1,2,...] [Ch1,Ch2,...]...
/**************************************/

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

void IV_channel(){
  const double VStart=0;
  double VStop=500;
  const double VStep=10;
  const double RangeDown=0;
  const double RangeUp=1000;
  char Filename[200];
  char Title[200];
  vector<string> Celltype;
  char level[20]="nA";
  vector<int> Wafer;
  vector<int> Color{1,2,3,4,6,7,8,9,11,12,28,30,36,38,40,42,46};
  vector<int> Style{20,21,22,28,29,39,47};
  vector<string> Date{}; 
  int argc, ai, ad, readN;
  bool as=false;
  char *p; 
  char const *delim=",";
  argc=gApplication->Argc();
  for(ai=0;ai<argc;++ai){
    //printf("a[%d]=%s\n", ai, gApplication->Argv(ai));
    if(as){
      p = strtok(gApplication->Argv(ai),delim);
      while(p){
        if(ai>ad){
          if(ai>ad+1){
            Celltype.push_back(p);
          }
          else{
            readN = atoi(p);
            Wafer.push_back(readN);
          }
        }
        else
          Date.push_back(p);
        p = strtok(NULL,delim);
      }
    }
    if(!strcmp(gApplication->Argv(ai),"@")){
      as=true;
      ad=ai+1;
    }
  }
  //Read data
  double Vr, Ir;
  map< array<int,3>,vector<double>> V;
  map< array<int,3>,vector<double>> I;
  array<int,3> key;
  vector<double> VV;
  vector<double> II;
  for(int wafer=0;wafer<Wafer.size();++wafer){
    for(int type=0;type<Celltype.size();++type){
      for(int date=0;date<Date.size();++date){
        if(atoi(Date[date].c_str())>191100)
          sprintf(Filename,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/ProbeCard/HGcal_6inch_%02d/%s/IV.txt",Date[date].c_str(), Wafer[wafer], Celltype[type].c_str());
        else
          sprintf(Filename,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s_NCU/KEI_2410/HGcal_6inch_%02d/%s/IV.txt",Date[date].c_str(), Wafer[wafer], Celltype[type].c_str());
        //cout << Filename << endl;
        fstream r1;
        r1.open(Filename,ios::in);
        VV.clear();
        II.clear();
        if(r1){
          key={wafer,type,date};
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
    }//type
  }//wafer
  TCanvas *c1 = new TCanvas("c1","IV",1024,768);
  TCanvas *c2 = new TCanvas("c2","Log IV",1024,768);
  gStyle-> SetOptStat(0);
  c1-> cd();
  c2-> SetLogy();

  for(int wafer=0;wafer<Wafer.size();wafer++){
    TLegend *leg = new TLegend(0.2,0.5,0.5,0.9,NULL,"brNDC");
    leg->SetBorderSize(0);
    leg->SetTextFont(62);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(1001);
    TMultiGraph *mg = new TMultiGraph();
    for(int type=0;type<Celltype.size();++type){
      for(int date=0;date<Date.size();++date){
        key = {wafer,type,date};
        TGraph* g1=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
        TGraph* g2=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
        sprintf(Title,"HGCal Wafer%d",Wafer[wafer]);
        g1-> SetTitle(Title);
        g1-> GetXaxis()-> SetTitle("V");
        g1-> GetYaxis()-> SetTitle(level);
        g1-> GetXaxis()-> SetLimits(VStart+VStep,VStop);
        c1->cd();
        g1-> GetYaxis()-> SetRangeUser(RangeDown,RangeUp);
        g1-> SetMarkerStyle(Style[date]);
        g1-> SetMarkerSize(1);
        g1-> SetMarkerColor(Color[type]);
        if(type==0 && date==0){
          g1-> Draw("ap");
          g1-> Draw("p");
        }
        else{
          g1-> Draw("p,same");
        }
        sprintf(Filename,"Channel%s %s",Celltype[type].c_str(),Date[date].c_str());
        leg-> AddEntry(g1,Filename,"p");
        c2-> cd();
        g2-> SetTitle(Title);
        g2-> GetXaxis()-> SetTitle("V");
        g2-> GetYaxis()-> SetTitle(level);
        g2-> GetXaxis()-> SetLimits(VStart+VStep,VStop);
        g2-> GetYaxis()-> SetRangeUser(10,10000);
        g2-> SetMarkerStyle(Style[date]);
        g2-> SetMarkerSize(1);
        g2-> SetMarkerColor(Color[type]);
        if(type==0 && date==0){
          g2-> Draw("ap");
          g2-> Draw("p");
        }
        else
          g2-> Draw("p,same");
        //mg-> Add(g1);
      }//end day loop
    }//end type loop
    //mg-> Draw("ap");
    //mg-> Draw("p");
    c1-> cd();
    leg-> Draw();
    sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2/w%d_Center%s_%.0lf.png",Wafer[wafer],Celltype[3].c_str(),RangeUp);
    c1-> Print(Filename);
    c2-> cd();
    leg-> Draw();
    sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2/w%d_Center%s_log.png",Wafer[wafer],Celltype[3].c_str());
    c2-> Print(Filename);
  }//end wafer loop

}
