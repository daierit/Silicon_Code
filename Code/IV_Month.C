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
#include <map>
using namespace std;

void IV_Month(){
  vector<int> Wafer{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
  vector<int> Color{1,2,3,4,6,7,8,9,11,12,28,30,36,38,40,42,46};
  vector<int> Style{20,21,22,23};
  vector<vector<string>> Date{{"190624_NCU"},
    {"190702_NCU","190711_NCU","190716_NCU","190725_NCU"},
    {"190801_NCU","190808_NCU"}};
  vector<string> DateTitle{"2019 Jun 24","2019 Jul 2,11,16,25","2019 Aug 1,8"}; 
  const double VStart=0;
  double VStop[2]={50,400};
  const double VStep=10;
  const double RangeDown=0;
  const double RangeUp=1000;
  char Filename[200];
  char Title[200];
  vector<string> Celltype{"014","058"};
  char level[20]="nA";
  //Read data
  double Vr, Ir;
  map< array<int,4>,vector<double>> V;
  map< array<int,4>,vector<double>> I;
  array<int,4> key;
  vector<double> VV;
  vector<double> II;
  for(int type=0;type<Celltype.size();++type){
    for(int wafer=0;wafer<Wafer.size();++wafer){
      for(int month=0;month<Date.size();++month){
        for(int day=0;day<Date[month].size();++day){
          sprintf(Filename,"/home/kycheng/Silicon_Sensor/HGcal_Sensor/%s/KEI_2410/HGcal_6inch_%02d/%s/IV.txt",Date[month][day].c_str(), Wafer[wafer], Celltype[type].c_str());
          //cout << Filename << endl;
          fstream r1;
          r1.open(Filename,ios::in);
          VV.clear();
          II.clear();
          if(r1){
            key={type,wafer,month,day};
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
        }//day
      }//month
    }// No loop end
  }// type loop end
  TCanvas *c1 = new TCanvas("c1","IV",1024,768);
  TCanvas *c2 = new TCanvas("c2","Log IV",1024,768);
  gStyle-> SetOptStat(0);
  c1-> cd();
  c2-> SetLogy();

  for(int type=0;type<Celltype.size();++type){
    for(int wafer=0;wafer<Wafer.size();++wafer){
      TLegend *leg = new TLegend(0.12,0.6,0.4,0.85,NULL,"brNDC");
      leg->SetBorderSize(0);
      leg->SetTextFont(62);
      leg->SetLineColor(1);
      leg->SetLineStyle(1);
      leg->SetLineWidth(1);
      leg->SetFillColor(0);
      leg->SetFillStyle(1001);
      TMultiGraph *mg = new TMultiGraph();
      for(int month=0;month<Date.size();month++){
        for(int day=0;day<Date[month].size();++day){
          key = {type,wafer,month,day};
          TGraph* g1=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
          TGraph* g2=new TGraph(V[key].size(),&V[key][0], &I[key][0]);
          sprintf(Title,"HGCal Wafer%d Ch%s",Wafer[wafer],Celltype[type].c_str());
          g1-> SetTitle(Title);
          g1-> GetXaxis()-> SetTitle("V");
          g1-> GetYaxis()-> SetTitle(level);
          g1-> GetXaxis()-> SetLimits(VStart+VStep,VStop[type]);
          c1->cd();
          g1-> GetYaxis()-> SetRangeUser(RangeDown,RangeUp);
          g1-> SetMarkerStyle(Style[day]);
          g1-> SetMarkerSize(1);
          g1-> SetMarkerColor(Color[month]);
          if(month==0){
            if(day==0){
              g1-> Draw("ap");
              g1-> Draw("p");
              leg-> AddEntry(g1,DateTitle[month].c_str(),"p");
            }
            //else
              //leg-> AddEntry(g1,"","p");
          }
          else{
            if(day==0)
              leg-> AddEntry(g1,DateTitle[month].c_str(),"p");
            g1-> Draw("p,same");
          }
          c2-> cd();
          g2-> SetTitle(Title);
          g2-> GetXaxis()-> SetTitle("V");
          g2-> GetYaxis()-> SetTitle(level);
          g2-> GetXaxis()-> SetLimits(VStart+VStep,VStop[type]);
          g2-> GetYaxis()-> SetRangeUser(10,10000);
          g2-> SetMarkerStyle(Style[day]);
          g2-> SetMarkerSize(1);
          g2-> SetMarkerColor(Color[month]);
          if(month==0 && day==0){
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
      c1-> cd();
      leg-> Draw();
      sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2_Month/%s_w%d_%.0lf.png",Celltype[type].c_str(),Wafer[wafer],RangeUp);
      c1-> Print(Filename);
      c2-> cd();
      leg-> Draw();
      sprintf(Filename,"/home/kycheng/Silicon_plot/HGCalRun2_Month/%s_w%d_log.png",Celltype[type].c_str(),Wafer[wafer]);
      c2-> Print(Filename);
    }//end wafer loop
  }//end type loop

}
